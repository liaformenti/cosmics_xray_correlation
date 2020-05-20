#ifndef RunAnalysis_h
#define RunAnalysis_h

// C++ includes
#include <iostream>
#include <map>
#include <string>

// Root includes
#include <TTree.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>
#include <TTreeReaderArray.h>
#include <TCanvas.h>

// tgc_analysis includes
#include "AnalysisInfo.h"
#include "DetectorGeometry.h"

// My includes
#include "Tracking.h"
#include "Residual.h"

void RunAnalysis(TTree &trksTree, AnalysisInfo &info, DetectorGeometry* _g);

// Helper fcn for RunAnalysis
Bool_t MissingHitsOnFixedLayers(UShort_t fixed1, UShort_t fixed2, std::map<UShort_t, Double_t> &xTrack, std::map<UShort_t, Double_t> &yTrack); 
void getOtherLayers(Int_t la, Int_t lb, Int_t* lc, Int_t* ld); 

#endif
