// Class for creating histograms to study bin sizes
#ifndef StatsStudy_h
#define StatsStudy_h

// C++ includes
#include <iostream>
#include<vector>
// #include <pair>

// Root includes
#include <TCanvas.h>
#include <TH2F.h>

// tgc_analysis includes
#include "DetectorGeometry.h"
#include "PlotManager.h"

// My includes
#include "Residual.h"

class StatsStudy {
  public:
    StatsStudy(){};
    StatsStudy(std::vector<Residual>* _residuals,
               DetectorGeometry* _g,
               PlotManager* _pm);
    ~StatsStudy(){};

  // Members
  void InitializeSquareBinHistograms(Double_t width);

  private:
    std::vector<Residual>* residuals = nullptr;
    DetectorGeometry* g=nullptr;
    PlotManager* pm=nullptr;
};
#endif
