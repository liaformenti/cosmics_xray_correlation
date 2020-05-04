#ifndef Tracking_h
#define Tracking_h

// C++ includes
#include <map>

// tgc_analysis includes
#include "DetectorGeometry.h"
#include "DetectorGeometryTools.h"

//Root includes

// My includes

class Tracking {
  public:
    // Constructors
    Tracking(){};
    Tracking(DetectorGeometry* _g, 
             std::map<UShort_t, Double_t> hitsMapX, 
             std::map<UShort_t, Double_t>* trackMapX, 
             std::map<UShort_t, Double_t> hitsMapY, 
             std::map<UShort_t, Double_t>* trackMapY, 
             UShort_t fixedLayer1, UShort_t fixedLayer2);
    ~Tracking(){};

    // Members
   // DetectorGeometry &g; 
   std::map<UShort_t, Double_t> hitsX; 
   std::map<UShort_t, Double_t>* trackX; 
   std::map<UShort_t, Double_t> hitsY; 
   std::map<UShort_t, Double_t>* trackY; 
   UShort_t la;
   UShort_t lb;
   
   // Methods
   void Track();
  private:
    DetectorGeometry* g=nullptr;
};
#endif
