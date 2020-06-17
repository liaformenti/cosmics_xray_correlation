// Class whose member is a map that contains bin edges for each layer
// Same bin edge arrays are in each layer key if same for all layers
// For uneven bins, bin width members (qw) are negative.
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
    Binning(Int_t xWidth, Int_t yWidth, DetectorGeometry* _g);
    ~Binning(){};

    // Members
    std::vector<Float_t> xBinEdges;
    std::vector<Float_t> yBinEdges;
    Int_t nBinsX;
    Int_t nBinsY; 
    // Old
    // std::map<UShort_t, std::vector<Float_t>> xBinEdges;
    // std::map<UShort_t, std::vector<Float_t>> yBinEdges;
    // std::vector<std::vector<Float_t>> xBinEdges;
    // std::vector<std::vector<Float_t>> yBinEdges;
    // Bin widths in mm initialized to negative and remain so unless
    // widths for binning scheme are fixed (re-init in constructor)
    // Int_t xw = -1; 
    // Int_t yw = -1; 
    // std::vector<Int_t> xw = {-1};
    // std::vector<Int_t> yw = {-1};

  private:
    // Members
    DetectorGeometry* g=nullptr;

};
#endif
