#ifndef Tracking_h
#define Tracking_h

// C++ includes
#include <map>

//Root includes
#include <TGraph.h>
#include <TF1.h>
#include <TCanvas.h>

// tgc_analysis includes
#include "DetectorGeometry.h"
#include "DetectorGeometryTools.h"

// My includes

class Tracking {
  public:
    // Constructors
    Tracking(){};
    Tracking(DetectorGeometry* _g, 
             std::map<UShort_t, Double_t> hitsMapX, 
             std::map<UShort_t, Double_t> hitsMapY); 
    ~Tracking(){};
   // Members
   std::map<UShort_t, Double_t> hitsX; 
   std::map<UShort_t, Double_t> hitsY; 
   // Methods

  private:
    // Members
    DetectorGeometry* g=nullptr;
    // Methods
    // Double_t* MapToArray(std::map<UShort_t, Double_t>*);
    Double_t* GetHitPoints(Int_t la, Int_t lb, std::map<UShort_t, Double_t> hits);
    void WriteOut(Int_t eventNumber);
    Bool_t MissingHitsOnFixedLayers(UShort_t fixed1, UShort_t fixed2, std::map<UShort_t, Double_t> &xTrack, std::map<UShort_t, Double_t> &yTrack); 

};

class Track {
  public:
  // Constructors
  Track(){};
  ~Track(){};


  // Members
  Double_t* x;
  Double_t* y;
  Double_t* z;
  TGraph graphX;
  TF1* fitX;
  TGraph graphY;
  TF1* fitY;
  
  // Methods
  void Fit();
  // void Extrapolate();
};
#endif
