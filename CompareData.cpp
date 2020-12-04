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
    fitFcn = new TF1("gaussian", "[2]*TMath::Gaus(x, [0], [1])", -10, 10);
}

LocalData::~LocalData() {
    delete fitFcn;
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

    cout << hist->GetEntries() << '\n';

    // Fit histogram
    // *** YOUR CUSTOM FIT FCN DOES NOT WORK - HAVE TO PUT ESTIMATES FOR PARAMS. NOT SURE HOW
    // *** TO DO THIS EQUITABLY
    // *** PROBABLY FCN POINTERS ARE THE WAY TO GO HERE
    // fitResult = hist->Fit(fitFcn, "S"); // Proably want to add Q = quiet option here eventually ******
    fitResult = hist->Fit("gaus", "S");
    
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

void CompareData::DoComparison(){
  for (auto xr=xResiduals->begin(); xr!=xResiduals->end(); xr++) {
      LocalData currentPoint(*xr, pm);  
      cout << xr->tag << '\n';
      currentPoint.SetRectangularROIs(xWidth, yWidth);
      currentPoint.GroupCosmicResiduals(*cResiduals);
      currentPoint.DoCosmicResidualsFit();
      break; // ***** REMOVE THIS TO DO ALL XRAY POINTS
  }
};

void CompareData::MakeScatterPlot(){};
