#ifndef GetAnalysisInfo_h
#define GetAnalysisInfo_h
 
// Include file for fcn to get AnalysisInfo object

// You need to check if you actually need these things once you start writing.

// C++ includes
#include <iostream>
#include <string>
#include <sstream>

// ROOT includes
#include <TFile.h>
#include <TROOT.h>

// tgc_analysis includes
#include "AnalysisInfo.h"

AnalysisInfo* GetAnalysisInfo(TFile* inROOT);

#endif
