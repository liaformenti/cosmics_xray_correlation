#ifndef Tracking_h
#define Tracking_h

// C++ includes
#include <map>

// tgc_analysis includes
#include "DetectorGeometry.h"

//Root includes

// My includes

class Tracking {
  public:
    Tracking(){};
    Tracking(DetectorGeometry &g, 
             std::map<UShort_t, Double_t> &hitsMapX, 
             std::map<UShort_t, Double_t> &trackMapX, 
             std::map<UShort_t, Double_t> &hitsMapY, 
             std::map<UShort_t, Double_t> &trackMapY, 
             UShort_t fixedLayer1, UShort_t fixedLayer2);
    ~Tracking(){};
};
#endif
