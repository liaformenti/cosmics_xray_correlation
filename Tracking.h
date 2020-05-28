#ifndef Tracking_h
#define Tracking_h

// C++ includes
#include <map>
#include <string>

// Root includes
#include <TGraphErrors.h>
#include <TAxis.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TFitResult.h>
#include <TFitResultPtr.h>
#include <TMatrixD.h>

// tgc_analysis includes
#include "DetectorGeometry.h"
#include "DetectorGeometryTools.h"
#include "PlotManager.h"

// My includes
#include "Helper.h"

class Tracking {
  public:
    // Constructors
    Tracking(){};
    Tracking(DetectorGeometry* _g, 
             PlotManager* _pm,
             std::map<UShort_t, Double_t> hitsMapX, 
             std::map<UShort_t, Double_t> hitsUncertX,
             std::map<UShort_t, Double_t> hitsMapY, 
             std::map<UShort_t, Double_t> hitsUncertY,
             UShort_t fixedLayer1, UShort_t fixedLayer2);
    ~Tracking(){};

   // Members
   // Tracks from tracks ttree become hits for this round of tracking
   std::map<UShort_t, Double_t> hitsX; 
   std::map<UShort_t, Double_t> hitsXUncerts;
   std::map<UShort_t, Double_t> fitXPos; 
   std::map<UShort_t, Double_t> fitXUncerts; 
   TGraphErrors graphX;
   TF1* fitXFcn;
   TFitResultPtr resultX;
   std::map<UShort_t, Double_t> hitsY; 
   std::map<UShort_t, Double_t> hitsYUncerts;
   std::map<UShort_t, Double_t> fitYPos; 
   std::map<UShort_t, Double_t> fitYUncerts; 
   TGraphErrors graphY;
   TF1* fitYFcn;
   TFitResultPtr resultY;
   UShort_t la;
   UShort_t lb;

   // Should make this add to plot manager
   void PlotFit(std::string outName); 
   
   // Methods
   void Fit();
   // For interpolation / extrapolation
   void EvaluateAt(UShort_t layer);
   // To makes plots of uncertainties in evaluated positions
   void InitializeUncertaintyHistograms();
  private:
    DetectorGeometry* g=nullptr;
    PlotManager* pm=nullptr;
    Double_t* MapToArray(std::map<UShort_t, Double_t>*);
};
#endif
