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
    // in mm. Note that lc should always be the layer with the 
    // smaller numerical value.
    CombinedData(Int_t wx, Int_t wy, Double_t _x, Double_t _y, 
                 UShort_t _lc, UShort_t _ld, 
                 Double_t _offC, Double_t _offD, 
                 std::vector<Residual>* _resData,
                 DetectorGeometry* _g, PlotManager* _pm);
    ~CombinedData(){};

  private:
    // Members
    // To be filled sequentially by methods
    Float_t x, y; // nominal xray x and y
    // x and y lims of region of interest centered around position:
    std::pair<Float_t, Float_t> xROI, yROI;
    // Flag swapped to true if ROI exceeds module limits
    Bool_t outOfRange = false; 
    UShort_t lc, ld; // layers of offsets used in difference
    Double_t offC, offD; // Offsets for layers A and B
    Double_t offDiff; // Difference in layer A and B's offsets
    // First pair is x lims around xray x, second pair is ylims
    // Can make a constructor that takes ROI instead of widths
    // to vary bin size point by point
    std::pair<Float_t, Float_t> ROI[2];
    // Vector to hold residuals that fall in ROI, for fixed layers
    // corresponding to difference
    std::vector<Double_t> residualsInROIC;
    std::vector<Double_t> residualsInROID;
    Double_t meanC, meanD; // Mean of Gaussian fit to residuals
    Double_t sigmaC, sigmaD;
    Double_t meanDiff; // Difference in means
    Double_t meanDiffUncert; // Propagated error in difference 
    std::vector<Residual>* resData = nullptr;
    DetectorGeometry* g = nullptr;
    PlotManager* pm = nullptr;

    // Methods
    // Returns point to an array of 2 pairs with x lim and y lim for 
    // around xray point position, x and y.
    void DefineRectangularROI(Int_t wx, Int_t wy);
};
#endif
