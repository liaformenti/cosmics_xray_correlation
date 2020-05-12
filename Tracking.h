#ifndef Tracking_h
#define Tracking_h

// C++ includes
#include <map>
#include <string>

//Root includes
#include <TGraphErrors.h>
#include <TAxis.h>
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
             std::map<UShort_t, Double_t> hitsUncertX,
             std::map<UShort_t, Double_t> trackMapX, 
             std::map<UShort_t, Double_t> trackUncertsX,
             std::map<UShort_t, Double_t> hitsMapY, 
             std::map<UShort_t, Double_t> hitsUncertY,
             std::map<UShort_t, Double_t> trackMapY, 
             std::map<UShort_t, Double_t> trackUncertsY,
             UShort_t fixedLayer1, UShort_t fixedLayer2);
    ~Tracking(){};

   // Members
   // Tracks from tracks ttree become hits for this round of tracking
   std::map<UShort_t, Double_t> hitsX; 
   std::map<UShort_t, Double_t> hUncertsX;
   std::map<UShort_t, Double_t> trackX; 
   std::map<UShort_t, Double_t> tUncertsX; 
   TGraphErrors graphX;
   TF1* fitX;
   std::map<UShort_t, Double_t> hitsY; 
   std::map<UShort_t, Double_t> hUncertsY;
   std::map<UShort_t, Double_t> trackY; 
   std::map<UShort_t, Double_t> tUncertsY; 
   TGraphErrors graphY;
   TF1* fitY;
   UShort_t la;
   UShort_t lb;

   void PlotFit(std::string outName);
   
   // Methods
   void Fit();
   // For interpolation / extrapolation
   void EvaluateAt(Double_t z);
  private:
    DetectorGeometry* g=nullptr;
    Double_t* MapToArray(std::map<UShort_t, Double_t>*);
};
#endif
