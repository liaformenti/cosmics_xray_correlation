#define CombinedData_cxx
#include "CombinedData.h"

using namespace std;

CombinedData::CombinedData(XRayPt xPt, UShort_t layerA,
  UShort_t layerB, vector<Residual>* _resData, DetectorGeometry* _g,
  PlotManager* _pm) : resData(_resData), g(_g), pm(_pm) {
    // Copy argument data into members
    xPtIndex = xPt.num;
    x = xPt.xnom;
    y = xPt.ynom;   
   
    // Check that offsets exist for each input layer
    // If not throw exception and prevent construction
    if (xPt.offsets.find(layerA) == xPt.offsets.end()) {
        throw invalid_argument("Invalid argument: no offset exists for layerA, " + to_string(layerA) + ".\n\n");
    }
    else if (xPt.offsets.find(layerB) == xPt.offsets.end()) {
        throw invalid_argument("Invalid argument: no offset exists for layerB, " + to_string(layerB) + ".\n\n");
    }

    // Assign smaller layer to la, larger to lb (consistency)
    if (layerA < layerB) {
        la = layerA;
        lb = layerB;
    }
    else if (layerA > layerB) {
        la = layerB;
        lb = layerA;
    }
    else { // they are equal
        throw invalid_argument("Invalid argument: both layers are the same (CombinedData constructor).\n\n");
    }
    
    // Get fixed layers, lc, ld, from layers of interest, la, lb.
    getOtherLayers(la, lb, &lc, &ld);

    // Init layer spec data
    PtLayerData first, second;
    first.offset = xPt.offsets.at(la);
    first.offsetError = xPt.offsetErrors.at(la);
    second.offset = xPt.offsets.at(lb);
    second.offset = xPt.offsetErrors.at(lb);

    first.residualsInROI = {}, second.residualsInROI = {};
    // Use fit false as a marker for before fit and failed fit
    first.success = false, second.success = false;
    // Init fit params to zero
    first.mean = 0, second.mean = 0;
    first.meanError = 0, second.meanError = 0;
    first.sigma = 0, second.sigma = 0;
    first.sigmaError = 0, second.sigmaError = 0;
    first.amplitude = 0, second.amplitude = 0;

    // Put points in map
    layerData.insert(pair<UShort_t, PtLayerData>(la, first));
    layerData.insert(pair<UShort_t, PtLayerData>(lb, second));

    // Calculate difference in xray offsets
    offDiff = layerData.at(la).offset - layerData.at(lb).offset;
    offDiffError = sqrt(pow(layerData.at(la).offsetError, 2) +
                        pow(layerData.at(lb).offsetError, 2));

    // Init residual mean difference to zero
    meanDiff = 0;
    meanDiffError = 0;

    return;
}
