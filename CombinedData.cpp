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
    // Check la < lb; Check that offsets exist in xPt for la and lb
    // For latter, if not, constructor must fail == throw exception?
    la = layerA;
    lb = layerB;
    // Get fixed layers, lc, ld, from layers of interest, la, lb.
    getOtherLayers(la, lb, &lc, &ld);
    // Start init'ing layer spec data
    PtLayerData first;
    first.offset = xPt.offsets.at(la);
    // Still need x/yROI, layerData map, offDiff & offDiffError
    return;
}
