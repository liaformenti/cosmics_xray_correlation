#ifndef Helper_h
#define Helper_h

// Useful functions not belonging to any class

// C++ includes
#include <iostream>
#include <string>
#include <map>
#include <unordered_map>

// Root includes
#include <TROOT.h>
#include <TH1F.h>
#include <TF1.h>
#include <TMatrixD.h>
#include <TVectorD.h>

// tgc_analysis includes
#include "DetectorGeometryTools.h"
// A "Combination" is 2 fixed layers and the layer of interest
struct Combination {
  Combination(){};
  Combination(UShort_t theLayer, UShort_t firstFixedLayer, UShort_t secondFixedLayer) : layer(theLayer), fixed1(firstFixedLayer), fixed2(secondFixedLayer){};
  UShort_t layer;
  UShort_t fixed1;
  UShort_t fixed2;
  std::string String() const;
  void Print();
  bool operator==(const Combination& c) const; 
  bool operator!=(const Combination& c) const;
};

std::vector<Combination> combinationVector(); 

// Data info contains the quadruplet name (eg. QS3P07), the sqlite3 (preprocessed) database with the
// xray data, the output directory must exist and its name should likely include the quadname and the 
// run voltage of the input datafile. 
struct InputInfo { 
  InputInfo(){};
  InputInfo(std::string theQuadName, std::string theDatabase, std::string theOutPath, std::string theTag);
  std::string quadname;
  std::string database;
  std::string outpath;
  std::string tag; // used to name files
};

void getOtherLayers(UShort_t la, UShort_t lb, UShort_t* lc, UShort_t* ld);

// For fitting clusters
// Modified from GausFitInfo struct in tgc_analysis
struct GausFitInfo{
  Bool_t fitResult = false; // true for successful fit
  Double_t A = std::numeric_limits<Double_t>::quiet_NaN();
  Double_t Aerr = std::numeric_limits<Double_t>::quiet_NaN();
  Double_t mean = std::numeric_limits<Double_t>::quiet_NaN();
  Double_t meanErr = std::numeric_limits<Double_t>::quiet_NaN();
  Double_t sigma = std::numeric_limits<Double_t>::quiet_NaN();
  Double_t sigmaErr = std::numeric_limits<Double_t>::quiet_NaN();
  Int_t NDF = -1;
  Double_t chi2 = std::numeric_limits<Double_t>::quiet_NaN();
  
  void Print() const{
    std::cout<<"FitResult="<<fitResult<<std::endl;
    std::cout<<"A="<<A<<" ("<<Aerr<<")"<<std::endl;
    std::cout<<"mean="<<mean<<" ("<<meanErr<<")"<<std::endl;
    std::cout<<"sigma="<<sigma<<" ("<<sigmaErr<<")"<<std::endl;
    std::cout<<"NDF="<<NDF<<std::endl;
    std::cout<<"chi2="<<chi2<<std::endl;
  }
};


// For fitting clusters
// Modified from tgc_analysis Clustering::DoGausFitMinuit by Benoit
void DoGausFitMinuit(const std::vector<Double_t> &x,
			      const std::vector<Double_t> &y,
			      GausFitInfo &info, Bool_t verbose=false);

void DoGausFitGuos(const std::vector<Double_t> &x, const std::vector<Double_t> &y,
                   GausFitInfo &info, Bool_t verbose);

// Takes in value of residual and position of track. Back-calculates position of hit from inputs,
// then returns y_rel of hit
Double_t CalculateYRelFromResidual(UShort_t layer, Double_t residual, Double_t trackY, DetectorGeometry* g);
// for creating strings of the form:
// header_layerC_fixedLayersAB_footer
// string getPermPlotName(string header, string footer, UShort_t layer, UShort_t fixedLayer1, UShort_t fixedLayer2);

// A fit result struct based on Gaussian parameters, but should be good if you want to store
// double gauss. Might not end up using this.
/*struct myFitResult {
    Bool_t success;
    Double_t mean;
    Double_t meanError;
    Double_t sigma;
    Double_t sigmaError;
    Double_t amplitude;
};*/

#endif

