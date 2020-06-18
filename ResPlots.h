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

// tgc_analysis includes
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
               DetectorGeometry* _g,
               PlotManager* _pm);
    ~ResPlots(){};
 
    // Create members
    void CreateNumEntriesTH2Is(std::string nameBase);
    // Print members
    void PrintNumEntriesTH2Is(std::string nameBase, 
                              std::string filename);
    // void FillResidualTH1Fs();
  private:
    std::vector<Residual>* residuals = nullptr;
    Binning* binning = nullptr;
    DetectorGeometry* g = nullptr; 
    PlotManager* pm = nullptr;
    // Initialization methods
    void InitializeNumEntriesTH2Is(std::string nameBase);
    // void InitializeResidualTH1Fs(Int_t xBinWidth, Int_t yBinWidth);
};
#endif
