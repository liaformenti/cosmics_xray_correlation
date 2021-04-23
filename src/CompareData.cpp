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
        nEntries = 0;
        return;
    }

    nEntries = hist->GetEntries();

    // Fit histogram
    // Range of fit is also fit range
    // Based on histogram RMS
    // Should be in config
    // Since fit fcn is still based on gaus, the mean parameter is stil named "Mean"
    Double_t histMean = hist->GetMean();
    Double_t histRMS = hist->GetRMS();
    TF1* fit = new TF1("myGaus", "gaus", histMean - histRMS, histMean + histRMS);
    fit->SetParameter(0, 100); // Guess for amplitude
    fit->SetParameter(1, histMean); // Guess for mean
    fit->SetParameter(2, histRMS); // Guess for sigma
    fitResult = hist->Fit("myGaus", "SQRL");

    // Get results
    if (fitResult != 0) {
        cout << "Warning: fit failed. ";
        xRes.PrintResidual();
        nFitParams = 0;
        return;
    } 

    fitFcn = (TF1*)hist->GetFunction("myGaus");
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

  // Prepare pdf file to print fits to
  TCanvas* c = new TCanvas();
  string filename = myInfo->outpath + myInfo->tag + "cosmic_residuals_in_ROIs.pdf";
  c->Print((filename + "[").c_str());

  // gStyle->SetOptStat(0);
  // gStyle->SetOptFit(1111);
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

      // Print local data info to file
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

    // Print (linear) fit results on plot
    // gStyle->SetOptFit(1111);

    // On x axis: xray residual
    // On y axis: mean cosmics residual in ROI around xray point

    // First, put all combinations' residuals on one plot
    vector<Double_t> X, Y, eX, eY;
    for (auto ld=localDataVec.begin(); ld!=localDataVec.end(); ld++) {
        // Skip points where residuals fit failed 
        // or where mean cosmics residual error is too high
        // should be in config
        if ((ld->fitResult != 0) || (ld->meanCosmicsResidualError > 0.07)) continue; 
        X.push_back(ld->xRes.res);
        eX.push_back(ld->xRes.resErr); 
        Y.push_back(ld->meanCosmicsResidual);
        eY.push_back(ld->meanCosmicsResidualError);
    }
    string theName = "local_cosmic_and_xray_residuals_scatter"; 
    string theTitle = "Comparing residuals - all tracking combinations;";
    // string theTitle = ";";
    theTitle += "Exclusive residual from x-ray data [mm];";
    theTitle += "Mean local exclusive residual from cosmics [mm];";
    // Create TGraphErrors
    pm->Add(theName, theTitle, X, Y, eX, eY, myTGraphErrors);
    // Get TGraphErrors 
    TGraphErrors* allLocalDataGraph = (TGraphErrors*)pm->Get(theName);
    // allLocalDataGraph->SetMarkerStyle(kCircle);
    if (allLocalDataGraph->GetN() == 0) {
        cout << "Warning: no association between x-ray retracking combinations and local cosmics data";
        cout << " (CompareData::MakeScatterPlot)\n\n";
        return;
    }
    allLocalDataGraph->Draw("AP");
    TF1* linFitAll = (TF1*)(gROOT->GetFunction("pol1"));
    linFitAll->SetParameters(0,1); // Resonable guesses for intercept, slope
    linFitAll->SetParNames("Offset", "Slope");
    allLocalDataGraph->Fit(linFitAll, "Q"); // Removed the F option for Minuit fitter
    c->Print(filename.c_str());
    c->Clear();

    // Now make tracking layer specific combinaton-specific plots
    vector<Combination> combVec = combinationVector();
    for (auto comb=combVec.begin(); comb!=combVec.end(); comb++) {
        vector<Double_t> x, y, ex, ey;        
        for (auto ld=localDataVec.begin(); ld!=localDataVec.end(); ld++) {
            // Skip data points with wrong combination
            if (*comb != ld->xRes.GetCombo()) continue;
            // Skip points where residuals fit failed or 
            // where mean cosmics residual error is too high
            // should be in config
            if ((ld->fitResult != 0) || (ld->meanCosmicsResidualError > 0.07)) continue; 
            // For correct combination, add data to vectors
            x.push_back(ld->xRes.res);
            ex.push_back(ld->xRes.resErr); 
            y.push_back(ld->meanCosmicsResidual);
            ey.push_back(ld->meanCosmicsResidualError);
        }
        // Create combination specific TGraphErrors
        string name = "local_cosmic_and_xray_residuals_scatter_" + comb->String();
        string title = "Comparing residuals - layer: " + to_string(comb->layer) + ", fixed layers: ";
        // string title = ";";
        title += to_string(comb->fixed1) + to_string(comb->fixed2);
        title += ";Exclusive residual from x-ray data [mm];";
        title += "Mean local exclusive residual from cosmics [mm];";
        pm->Add(name, title, x, y, ex, ey, myTGraphErrors); 
        // Get TGraphErrors
        TGraphErrors* localDataGraph = (TGraphErrors*)pm->Get(name);
        if (localDataGraph->GetN() != 0) {
            TF1* linFit = (TF1*)(gROOT->GetFunction("pol1"));
            linFit->SetParameters(0,1); // Resonable guesses for intercept, slope
            linFit->SetParNames("Offset", "Slope");
            localDataGraph->Fit(linFit, "Q"); // Removed the F option for Minuit fitter
            localDataGraph->Draw("AP");
            c->Print(filename.c_str());
            c->Clear();
        }
    } // End combo loop 

    c->Print((filename + "]").c_str());
    delete c; 
    return;
}

void CompareData::OutputLocalDataToCSV() {
    
  // Prepare csv file to print numerical results to
  ofstream f;
  f.open(myInfo->outpath + myInfo->tag + myInfo->quadname + "_local_cosmic_and_xray_data.csv");
  f << "X-ray pt id,Layer,Fixed layer 1,Fixed layer 2,x,y,x low,x high,y low,y high,";
  f << "x-ray residual,xray residual error,";
  f << "fit result,nEntries,";

  // Get header
  for (auto ld=localDataVec.begin(); ld!=localDataVec.end(); ld++) {
      // If the fit result failed, the headers won't have been init'ed for this ld, so skip it.
      if (ld->fitResult != 0) continue;
      for (auto name=ld->fitParamNames.begin(); name!=ld->fitParamNames.end(); name++) {
          f << *name << ',' << *name << " error,"; 
      }
      break; // Only need header printed once
  }
  f << '\n';

  // Now ouput fit data to file
  for (auto ld=localDataVec.begin(); ld!=localDataVec.end(); ld++) {
    f << ld->xRes.tag << ',' << ld->xRes.l << ',' << ld->xRes.la << ',' << ld->xRes.lb;
    f << ',' << ld->xRes.x << ',' << ld->xRes.y << ',' << ld->xROI.first << ',' << ld->xROI.second;
    f << ',' << ld->yROI.first << ',' << ld->yROI.second << ',' << ld->xRes.res << ',';
    f << ld->xRes.resErr << ',' << ld->fitResult << ',' << ld->nEntries << ',';
    
    // If fit failed, or there are no entries
    if (ld->fitResult != 0 || ld->nEntries == 0) {
      for (Int_t i=0; i<ld->nFitParams; i++) {
          f << "NA,NA,";
      }
      f << '\n';
    } 
    else { // if fit was successful,
      for (Int_t i=0; i<ld->nFitParams; i++) {
        f << ld->fitParamValues.at(i) << ',' << ld->fitParamErrors.at(i) << ',';
      }
      f << '\n';
    }
  }
  
  f.close();
  return;
}
