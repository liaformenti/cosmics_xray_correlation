// Takes in tracks TTree from cosmics and recreates tracks based on hits from two fixed layers,
// for all permutations.
// Records the residuals (Residual class)
// Oct 25, 2020

#ifndef CosmicsRetracking_h
#define CosmicsRetracking_h

// C++ includes
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <fstream>

// Root includes
#include <TTree.h>
#include <TCanvas.h>

// tgc_analysis includes
#include "AnalysisInfo.h"
#include "DetectorGeometry.h"
#include "PlotManager.h"

// My includes
#include "Tracking.h"
#include "Residual.h"
#include "Helper.h"
#include "Retracking.h"
// #include "Binning.h"
// #include "ResPlots.h"
// #include "XRayData.h"
// #include "CombinedData.h"

class CosmicsRetracking : public Retracking {
  public:
    // CosmicsRetracking(){};
    CosmicsRetracking(TTree* _trksTree, AnalysisInfo* _cInfo, InputInfo* _myInfo, 
                      PlotManager* _pm, DetectorGeometry* _g);
    ~CosmicsRetracking(){};
    
    // Members
    Int_t nEntries; // Number of tracks in tree
    // std::vector<Tracking> xTracks; // Store retracked tracks here
    // std::vector<Tracking> yTracks;
    // std::vector<Residual> residuals;
    
    void Retrack(); // Actually does the retracking and applies any cuts. Fills residuals vector.
    // Use private la lb members
    // Bool_t MissingHitsOnFixedLayers(std::map<UShort_t, Double_t> &xTrack, 
    //                                std::map<UShort_t, Double_t> &yTrack);
    void PrintTrackUncertaintyHistograms();
    void PrintTrackAngleHistograms();

  private:
    TTree* trksTree = nullptr;
    // AnalysisInfo* cInfo = nullptr;
    // InputInfo* myInfo = nullptr;
    // PlotManager* pm = nullptr;
    // DetectorGeometry* g = nullptr;
    // UShort_t la=0, lb=0; // store fixed layers for a given track
    // UShort_t lc=0, ld=0; // store layers to evaluate to for a given track
    void InitializeTrackUncertaintyHistograms();
    void InitializeTrackAngleHistograms();
};

#endif


