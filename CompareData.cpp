#define CompareData_cxx
#include "CompareData.h"

using namespace std;

LocalData::LocalData(Residual xrayResidual, Double_t xBinWidth, Double_t yBinWidth) :
                     xRes(xrayResidual) {
    cout << xRes.res << '\n';
    SetRectangularROIs(xBinWidth, yBinWidth);
    
    // Create method that does grouping.
}

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
      LocalData(*xr, xWidth, yWidth);  
      break;
  }
};

void CompareData::MakeScatterPlot(){};
