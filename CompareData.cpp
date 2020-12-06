#define CompareData_cxx
#include "CompareData.h"

using namespace std;

LocalData::LocalData(Residual xrayResidual, PlotManager* _pm) :
                     xRes(xrayResidual), pm(_pm) {
    // Initialize region of interest so that methods can check if they have been set
    // before use.
    xROI.first = 0;
    xROI.second = 0;
    yROI.first = 0;
    yROI.second = 0;

    // Default fit fcn is Gaussian
    // Use ROOT's predefined gaus fit fcn
    // fitFcn = new TF1("gaussian", "[2]*TMath::Gaus(x, [0], [1])", -10, 10);
}

LocalData::~LocalData() {
}

// Must set the ROI somehow before grouping cosmics.
void LocalData::SetRectangularROIs(Double_t xWidth, Double_t yWidth) {
    if ((xWidth <= 0) || (yWidth <= 0)) {
        throw invalid_argument("Please give positive widths (CombinedData::DefineRectangularROI).\n\n");
    }
    xROI.first = xRes.x - xWidth/2.0;
    xROI.second = xRes.x + xWidth/2.0;
    yROI.first = xRes.y - yWidth/2.0;
    yROI.second = xRes.y + yWidth/2.0;
    return;
}

void LocalData::GroupCosmicResiduals(const vector<Residual>& allCosmicResiduals) {
    // If ROIs are not assigned, they are all set to zero.
    // Check that they have been assigned to a meaningful quantity.
    Float_t eps = 0.1; // mm
    if ( ( (abs(xROI.second - xROI.first) < eps) ) ||
         ( (abs(yROI.second - yROI.first) < eps) ) ) {
        throw logic_error("Region of interest around xray point not defined (CombinedData::FillROIWithResiduals).\n\n");
    }
    for (auto r=allCosmicResiduals.begin(); r!=allCosmicResiduals.end(); r++) {
        // If residuals is not on right layer, skip.
        if (r->l != xRes.l) continue;
        // If fixed layers are not right, skip.
        if ((r->la != xRes.la) || (r->lb != xRes.lb)) continue;
        // If residual is in ROI
        if ( (r->x > xROI.first) && (r->x < xROI.second) &&
             (r->y > yROI.first) && (r->y < yROI.second) ) {
             cosmicResidualsInROI.push_back(r->res);
        }
    }
    return; 
}

void LocalData::DoCosmicResidualsFit() {
    // Throw error if cosmicResidualsInROI is empty
    if (cosmicResidualsInROI.size() == 0) 
        throw logic_error("No cosmic residuals in ROI. Check that GroupCosmicResiduals was called in CompareData::DoComparison (LocalData::DoCosmicResidualsFit).\n\n");

    // First, create histogram
    string name = "cosmic_residuals_around_xray_point_" + xRes.tag + "_" + xRes.GetCombo().String();
    string title = "X-ray point: " + xRes.tag + ", Layer: " + to_string(xRes.l); 
    title += ", Fixed layers: " + to_string(xRes.la) + to_string(xRes.lb);
    title += ", x#in["+Tools::CStr(xROI.first,2)+","+Tools::CStr(xROI.second,2)+"] mm y#in[";
    title += Tools::CStr(yROI.first,2)+","+Tools::CStr(yROI.second,2)+"] mm;Cosmic Residuals [mm];";
    title += "Tracks";
    pm->Add(name, title, 200, -10, 10, myTH1I);

    // Fill histogram
    for (auto cr=cosmicResidualsInROI.begin(); cr!=cosmicResidualsInROI.end(); cr++) {
        pm->Fill(name, *cr);
    }
    
    // Get histogram
    TH1I* hist = pm->GetTH1I(name);

    if (hist->GetEntries() < 1) {
        cout << "Warning: histogram of cosmic residuals in ROI is empty."; 
        cout << "Are your bin widths too small?\n\n";
        nFitParams = 0;
        return;
    }

    // Fit histogram
    fitResult = hist->Fit("gaus", "S");

    // Get results
    if (fitResult != 0) {
        cout << "Warning: fit failed. ";
        xRes.PrintResidual();
        nFitParams = 0;
        return;
    } 

    fitFcn = (TF1*)hist->GetFunction("gaus");
    nFitParams = fitFcn->GetNpar();
    
    // Put fit paramter names and values in member vectors
    for (Int_t i=0; i<nFitParams; i++) {
        fitParamNames.push_back(fitFcn->GetParName(i));
        fitParamValues.push_back(fitResult->Parameter(i));
        fitParamErrors.push_back(fitResult->ParError(i));
    }

    // Get mean cosmics residual
    // Find out which param is "Mean"
    Int_t meanParamIndex = -1;
    for (Int_t i=0; i<nFitParams; i++) {
        if (fitParamNames.at(i) == "Mean") {
            meanParamIndex = i;
            break;
        }
    } 
       
    // If you didn't find a parameter called "Mean", something is wrong with the supplied fit fcn
    if (meanParamIndex == -1)
        throw invalid_argument("Unable to find a parameter called \"Mean\" in the fit function. The fit function supplied must have a \"Mean\" to know which parameter to extract (LocalData::DoCosmicResidualsFit)\n\n");

    // AND THE RESULT!
    meanCosmicsResidual = fitParamValues.at(meanParamIndex);
    meanCosmicsResidualError = fitParamErrors.at(meanParamIndex);
    return;
}

CompareData::CompareData(Double_t xBinWidth, Double_t yBinWidth, vector<Residual>* _xRayResiduals, 
                         vector<Residual>* _cosmicsResiduals, InputInfo* _myInfo, PlotManager* _pm, 
                         DetectorGeometry* _g) : xResiduals(_xRayResiduals), 
                         cResiduals(_cosmicsResiduals), myInfo(_myInfo), pm(_pm), g(_g) {
    // Set public members
    xWidth = xBinWidth;
    yWidth = yBinWidth;
};

// Initializes localDataVec with comparison data
void CompareData::DoComparison() {

  // Prepare pdf file to print to
  TCanvas* c = new TCanvas();
  string filename = myInfo->outpath + myInfo->tag + "cosmic_residuals_in_ROIs.pdf";
  c->Print((filename + "[").c_str());

  for (auto xr=xResiduals->begin(); xr!=xResiduals->end(); xr++) {

      LocalData currentPoint(*xr, pm);  
      xr->PrintResidual();
      currentPoint.SetRectangularROIs(xWidth, yWidth);
      currentPoint.GroupCosmicResiduals(*cResiduals);
      currentPoint.DoCosmicResidualsFit();
      localDataVec.push_back(currentPoint);

      // Print hist and fit to PDF
      string name = "cosmic_residuals_around_xray_point_" + xr->tag + "_" + xr->GetCombo().String();
      TH1I* hist = pm->GetTH1I(name);
      if (hist->GetEntries() != 0) {
          hist->Draw();
          c->Print(filename.c_str());
          c->Clear();
      }
      cout << '\n';
  }

  c->Print((filename + "]").c_str());
  delete c;
  return;
}

void CompareData::MakeScatterPlot(){
    // Prep canvas and pdf
    TCanvas* c = new TCanvas();   
    string filename = myInfo->outpath + myInfo->tag; 
    filename += "local_mean_cosmics_residual_vs_xray_residual_scatter.pdf";
    c->Print((filename + "[").c_str());

    // On x axis: xray residual
    // On y axis: mean cosmics residual in ROI around xray point
    // First, put all combinations' residuals on one plot
    vector<Double_t> X, Y, eX, eY;
    for (auto ld=localDataVec.begin(); ld!=localDataVec.end(); ld++) {
        X.push_back(ld->xRes.res);
        eX.push_back(0); // Currently don't have errors on xray residuals *************
        Y.push_back(ld->meanCosmicsResidual);
        eY.push_back(ld->meanCosmicsResidualError);
    }
    string theName = "local_cosmic_and_xray_residuals_scatter"; 
    string theTitle = "Comparing residuals - all tracking combinations;";
    theTitle += "Exclusive residual from x-ray data [mm];";
    theTitle += "Mean local exclusive residual from cosmics [mm];";
    // Create TGraphErrors
    pm->Add(theName, theTitle, X, Y, eX, eY, myTGraphErrors);
    // Get TGraphErrors 
    TGraphErrors* allLocalDataGraph = (TGraphErrors*)pm->Get(theName);
    allLocalDataGraph->SetMarkerStyle(kCircle);
    allLocalDataGraph->Draw("AP");
    c->Print(filename.c_str());

    // Now make combinaton-specific plots
    vector<Combination> combVec = combinationVector();
    for (auto comb=combVec.begin(); comb!=combVec.end(); comb++) {
        vector<Double_t> x, y, ex, ey;        
        for (auto ld=localDataVec.begin(); ld!=localDataVec.end(); ld++) {
            if (*comb == ld->xRes.GetCombo()) { //******** TEST THIS
                cout << comb->layer << ' ' << comb->fixed1 << ' ' << comb->fixed2 << '\n';
                cout << ld->xRes.GetCombo().layer << ' ' << ld->xRes.GetCombo().fixed1 << ' ' << ld->xRes.GetCombo.fixed2 << '\n';
            }
        }
        break //************* REMOVE THIS
    } 

    c->Print((filename + "]").c_str());
    delete c; 
    return;
}
