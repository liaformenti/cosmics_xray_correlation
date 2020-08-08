#ifndef RunAnalysis_h
#define RunAnalysis_h

// C++ includes
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <fstream>

// Root includes
#include <TTree.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>
#include <TTreeReaderArray.h>
#include <TCanvas.h>
#include <TGraphErrors.h>
#include <TMultiGraph.h>

// tgc_analysis includes
#include "AnalysisInfo.h"
#include "DetectorGeometry.h"
#include "PlotManager.h"

// My includes
#include "Tracking.h"
#include "Residual.h"
#include "Helper.h"
#include "Binning.h"
#include "ResPlots.h"
#include "XRayData.h"
#include "CombinedData.h"

void RunAnalysis(TTree &trksTree, AnalysisInfo* cosmicsInfo, PlotManager* pm, DetectorGeometry* g, InputInfo* myInfo);

// Helper fcn for RunAnalysis
Bool_t MissingHitsOnFixedLayers(UShort_t fixed1, UShort_t fixed2, std::map<UShort_t, Double_t> &xTrack, std::map<UShort_t, Double_t> &yTrack); 
void initializeUncertaintyHistograms(PlotManager* pm);
void printUncertaintyHistograms(PlotManager* pm);
#endif
