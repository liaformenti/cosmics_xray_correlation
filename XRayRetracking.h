// Tracks xray offset / x beam profile across layers of sTGC
// Records the residuals (Residual Class)
// Nov 26, 2020

#ifndef XRayRetracking_h
#define XRayRetracking_h

// C++ includes
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <fstream>

// tgc_analysis includes
#include "AnalysisInfo.h"
#include "DetectorGeometry.h"
#include "PlotManager.h"

// My includes
#include "Tracking.h"
#include "Residual.h"
#include "Helper.h"
#include "XRayData.h"

class XRayRetracking {
  public:
    XRayRetracking(){};
    XRayRetracking(XRayData* _xData, AnalysisInfo* _cInfo, InputInfo* _myInfo, PlotManager* _pm, 
                   DetectorGeometry* _g);
    ~XRayRetracking(){};

    // Members
    std::vector<Tracking> xTracks;
    std::vector<Tracking> yTracks;
    std::vector<Residual> residuals;

    void Retrack(); // Actually does the retrackign and applies any cuts. Fills residuals vector

  private:
    XRayData* xData = nullptr;
    AnalysisInfo* cInfo = nullptr;
    InputInfo* myInfo = nullptr;
    PlotManager* pm = nullptr;
    DetectorGeometry* g = nullptr;
};

#endif
