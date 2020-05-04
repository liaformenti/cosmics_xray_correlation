#ifndef RunAnalysis_h
#define RunAnalysis_h

// C++ includes
#include <iostream>
#include <map>

// Root includes
#include <TTree.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>
#include <TTreeReaderArray.h>

// tgc_analysis includes
#include "AnalysisInfo.h"
#include "DetectorGeometry.h"

// My includes
#include "Tracking.h"

void RunAnalysis(TTree &trksTree, AnalysisInfo &info, DetectorGeometry &g);

// Helper fcn for RunAnalysis
Bool_t MissingHitsOnFixedLayers(UShort_t fixed1, UShort_t fixed2, std::map<UShort_t, Double_t> &xTrack, std::map<UShort_t, Double_t> &yTrack); 

#endif
