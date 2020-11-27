#define XRayRetracking_cxx
#include "XRayRetracking.h"

XRayRetracking::XRayRetracking(XRayData* _xData, AnalysisInfo* _cInfo, InputInfo* _myInfo, 
                               PlotManager* _pm, DetectorGeometry* _g) : xData(_xData), cInfo(_cInfo),
                               myInfo(_myInfo), pm(_pm), g(_g) {
}
