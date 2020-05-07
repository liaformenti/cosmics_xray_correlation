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

void RunAnalysis(TTree &trksTree, AnalysisInfo &info, DetectorGeometry* _g);

#endif
