// Class for creating plots filled with the residuals.
// Each plot type will have a create method, a name-building method,
// a print-to-pdf method, and a private initialization method called by
// the create method.
#ifndef ResPlots_h 
#define ResPlots_h

// C++ includes
#include <iostream>
#include <vector>
#include <math.h>
#include <string.h>

// Root includes
#include <TCanvas.h>
#include <TH2F.h>
#include <TH1F.h>
#include <TH1I.h>
#include <TH2I.h>
#include <TStyle.h>

// tgc_analysis includes
#include "AnalysisInfo.h"
#include "DetectorGeometry.h"
#include "PlotManager.h"
#include "Tools.h"

// My includes
#include "Residual.h"
#include "Helper.h"
#include "Binning.h"

class ResPlots {
  public:
    ResPlots(){};
    ResPlots(std::vector<Residual>* _residuals,
               Binning* _binning,
               std::string namebase,
               AnalysisInfo* _cinfo,
               DetectorGeometry* _g,
               PlotManager* _pm,
               InputInfo* _myInfo);
    ~ResPlots(){};
 
    // Create members
    std::string nameBase; // Used as prefix to plot names.
    // Each plot name is of the format:
    // namBase + unique_identifier + relevant_conditions
    // Titles are generic
    // Caution must be made in supplying a good pdf name in Prints
    void CreateNumEntriesTH2Is();
    void PrintNumEntriesTH2Is(std::string filename);
    void CreatePosBinnedResPlots();
    void PrintPosBinnedResPlots(std::string filename);
    void CreatePosBinnedFitResultTH2Fs();
    void PrintPosBinnedFitResultTH2Fs(std::string filename);
    // Print members
  private:
    std::vector<Residual>* residuals = nullptr;
    Binning* binning = nullptr;
    AnalysisInfo* cinfo = nullptr;
    DetectorGeometry* g = nullptr; 
    PlotManager* pm = nullptr;
    InputInfo* myInfo = nullptr;
    // Initialization methods
    void InitializeNumEntriesTH2Is();
    void InitializePosBinnedResPlots();
    void InitializePosBinnedFitResultTH2Fs();
};
#endif
