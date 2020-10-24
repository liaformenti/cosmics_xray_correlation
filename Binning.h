#ifndef Binning_h
#define Binning_h

// C++ includes
#include <iostream>
#include <vector>
#include <string.h>
#include <algorithm>

// Root includes
#include <Rtypes.h>
#include <TROOT.h>

// tgc_analysis includes
#include "DetectorGeometry.h"

// My includes
#include <XRayData.h>

class Binning {
  public:
    Binning(){};
    // Takes in bin widths in mm
    // Rectangular bins
    // Lowest y bin and highest x bin not width - controlled
    Binning(Int_t xWidth, Int_t yWidth, DetectorGeometry* _g);
    // To bin a rectangle around xray points, with specified
    // x and y widths
    // Binning(XRayData* data, Int_t xWidth, Int_t yWidth,
    //        DetectorGeometry* _g);
    // Creates rectangular bins of width yWidth [mm] in y and a given fraction of
    // the wire group width in x; centered around the wire groups. 
    // Still gotta figure this out and code it . . . 
    // Binning(Float_t xFrac, Int_t yWidth, DetectorGeometry* _g);
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
    // Methods
    // To define a bin around the centre, q
    // Adds bin lims to xBinEdges or yBinEdges depending on
    // string xOrY.
    // Assumes it will be called sequentially (order of increasing bin
    // limits) from XRayData based Binning constructor
    void DefineXRayBin(Float_t q, Float_t qmin, Float_t qmax, 
                       Int_t width, std::string xOrY);

};
#endif
