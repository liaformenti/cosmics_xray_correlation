// Class for creating 2D histograms to study no. entries / square bin
#ifndef StatsStudy_h 
#define StatsStudy_h

// C++ includes
#include <iostream>
#include<vector>
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

class StatsStudy {
  public:
    StatsStudy(){};
    StatsStudy(std::vector<Residual>* _residuals,
               DetectorGeometry* _g,
               PlotManager* _pm);
    ~StatsStudy(){};
 
    // Members
    Int_t wx; // width of bins in x (mm)
    Int_t wy; // width of bins in y (mm)
    void InitializeResidualTH1Fs(Int_t xBinWidth, Int_t yBinWidth);
    void FillResidualTH1Fs();
  private:
    std::vector<Residual>* residuals = nullptr;
    DetectorGeometry* g = nullptr; 
    PlotManager* pm = nullptr;
};
#endif
