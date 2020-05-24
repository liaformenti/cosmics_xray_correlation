#ifndef Residual_h
#define Residual_h

// C++ includes
#include <iostream>

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
    Residual(Double_t residual, UShort_t layer, 
             Double_t xpos, Double_t ypos, 
             UShort_t fixedLayer1, UShort_t fixedLayer2);
    Residual(Tracking &trackInfo, UShort_t layer);
    ~Residual(){};

    // Members
    Double_t res;
    UShort_t l;
    Double_t x;
    Double_t y;
    UShort_t la;
    UShort_t lb;
};
#endif
