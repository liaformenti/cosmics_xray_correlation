#ifndef CombinedData_h 
#define CombinedData_h

// C++ includes
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>

// tgc_analysis includes

// My includes
#include "XRayData.h"
#include "Helper.h"
#include "Residual.h"

// CombinedData organizes and analyses the xray data and fixed-layer
// based residuals data together.
// The first constructor compares differences in xray offsets 
// across layers to the difference in residuals means on those layers 
// in a region of interest (rectangular, defined in constructor).

class CombinedData {
  public:
    // Constructors
    CombinedData(){};
    // Takes in the data from an XRayData entry and methods fill
    // members. wx and wy are widths of regions of interest in x and y
    // in mm.
    CombinedData(Int_t wx, Int_t wy, Double_t _x, Double_t _y, 
                 UShort_t _lA, UShort_t _lB, 
                 Double_t _offA, Double_t _offB, 
                 std::vector<Residual>* _resData,
                 DetectorGeometry* _g, PlotManager* _pm);
    ~CombinedData(){};

  private:
    // To be filled sequentially by method
    Float_t x, y; // nominal xray x and y
    // x and y lims of region of interest centered around position:
    std::pair<Float_t, Float_t> xROI, yROI;
    // Flag swapped to true if ROI exceeds module limits
    Bool_t outOfRange = false; 
    UShort_t lA, lB; // layers of offsets used in difference
    Double_t offA, offB; // Offsets for layers A and B
    Double_t offDiff; // Difference in layer A and B's offsets
    // Vector to hold residuals that fall in ROI, for fixed layers
    // corresponding to difference
    std::vector<Double_t> residualsInROI;
    Double_t meanA, meanB; // Mean of Gaussian fit to residuals
    Double_t sigmaA, sigmaB;
    Double_t meanDiff; // Difference in means
    Double_t meanDiffUncert; // Propagated error in difference 
    std::vector<Residual>* resData = nullptr;
    DetectorGeometry* g = nullptr;
    PlotManager* pm = nullptr;
};
#endif
