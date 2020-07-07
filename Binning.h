#ifndef Binning_h
#define Binning_h

// C++ includes
#include <iostream>
#include <vector>
#include <string.h>

// Root includes
#include <Rtypes.h>
#include <TROOT.h>
// tgc_analysis includes
#include "DetectorGeometry.h"

// My includes

class Binning {
  public:
    Binning(){};
    // Takes in bin widths in mm
    // Rectangular bins
    // Lowest y bin and highest x bin not width - controlled
    Binning(Int_t xWidth, Int_t yWidth, DetectorGeometry* _g);
    ~Binning(){};

    // Members
    // xBinEdges and yBinEdges first entry is lower limit
    // last entry is upper limit
    // so xBinEdges.size() = nBinsX + 1
    std::vector<Float_t> xBinEdges;
    std::vector<Float_t> yBinEdges;
    Int_t nBinsX;
    Int_t nBinsY; 
    std::string name; // holds a string describing the binning, useful for naming schemes
  private:
    // Members
    DetectorGeometry* g=nullptr;

};
#endif
