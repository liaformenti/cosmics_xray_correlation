// Class for creating 2D histograms to study no. entries / square bin
#ifndef SquareBinResidualHistogramEntries_h
#define SquareBinResidualHistogramEntries_h

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

// My includes
#include "Residual.h"
#include "Helper.h"

class SquareBinResidualHistogramEntries {
  public:
    SquareBinResidualHistogramEntries(){};
    SquareBinResidualHistogramEntries(std::vector<Residual>* _residuals,
               DetectorGeometry* _g,
               PlotManager* _pm);
    ~SquareBinResidualHistogramEntries(){};

    // Members
    Int_t binWidth; // square bin width in mm
    // Methods
    void InitializeSquareBinHistograms(Int_t width);
    void FillSquareBinHistograms();
    void PrintSquareBinHistograms(std::string filename);   
  private:
    // Members
    std::vector<Residual>* residuals = nullptr;
    DetectorGeometry* g=nullptr;
    PlotManager* pm=nullptr;
    // Methods
    std::string GetSquareBinHistName(UShort_t layer, UShort_t fixedLayer1, UShort_t fixedLayer2);
    
};
#endif