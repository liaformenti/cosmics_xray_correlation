#ifndef Tracking_h
#define Tracking_h

// C++ includes
#include <map>
#include <string>

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
    /*Tracking(DetectorGeometry* _g, 
             std::map<UShort_t, Double_t> hitsMapX, 
             std::map<UShort_t, Double_t>* trackMapX, 
             std::map<UShort_t, Double_t> hitsMapY, 
             std::map<UShort_t, Double_t>* trackMapY, 
             UShort_t fixedLayer1, UShort_t fixedLayer2,
             Bool_t  writeOut, Int_t eventNum);*/
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
   TGraph graphX;
   TF1* fitX;
   std::map<UShort_t, Double_t> hitsY; 
   std::map<UShort_t, Double_t>* trackY; 
   TGraph graphY;
   TF1* fitY;
   UShort_t la;
   UShort_t lb;
   // void PlotFit(TCanvas* c, std::string name);
   void PlotFit(std::string name);
   
   // Methods
   void Fit();
  private:
    DetectorGeometry* g=nullptr;
    Double_t* MapToArray(std::map<UShort_t, Double_t>*);
};
#endif
