#ifndef Residual_h
#define Residual_h

// C++ includes
#include <iostream>
#include <math.h>

// Root includes
#include "Rtypes.h"

// tgc_analysis includes

// My includes
#include "Tracking.h"

// Class to store residual, layer it is on, position, 
// and which fixed layers it was calculated with respect to

class Residual {
  public:
    // Constructors
    Residual(){};
    Residual(Double_t residual, Double_t residualError, UShort_t layer, 
             Double_t xpos, Double_t ypos, 
             UShort_t fixedLayer1, UShort_t fixedLayer2);
    Residual(Tracking &trackInfo, UShort_t layer);
    ~Residual(){};

    // Members
    Double_t res;
    Double_t resErr;
    UShort_t l;
    Double_t x;
    Double_t y;
    UShort_t la;
    UShort_t lb;
    // Can be used to hold identifying information, such at platform ID and position number of xray pt
    std::string tag = ""; 
    Combination GetCombo();
    void PrintResidual();
};
#endif
