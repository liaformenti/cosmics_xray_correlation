#ifndef Retracking_h
#define Retracking_h

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

class Retracking {
  public:
    Retracking(){};
    Retracking(AnalysisInfo* _cInfo, InputInfo* _myInfo, 
               PlotManager* _pm, DetectorGeometry* _g);
    ~Retracking(){};
    
    // Members
    Int_t nEntries; 
    std::vector<Tracking> xTracks; // Store retracked tracks here
    std::vector<Tracking> yTracks;
    std::vector<Residual> residuals;
    
    virtual void Retrack() = 0; 
    // Use private la lb members
    Bool_t MissingHitsOnFixedLayers(std::map<UShort_t, Double_t> &xTrack, 
                                    std::map<UShort_t, Double_t> &yTrack);

  protected:
    AnalysisInfo* cInfo = nullptr;
    InputInfo* myInfo = nullptr;
    PlotManager* pm = nullptr;
    DetectorGeometry* g = nullptr;
    UShort_t la=0, lb=0; // Store fixed layers for a given track
    UShort_t lc=0, ld=0; // Store layers to evaluate to for a given track.
};

#endif
