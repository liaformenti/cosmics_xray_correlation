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

// tgc_analysis includes
#include "FitTools.h"

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

// From tgc_analysis FinalizeAnalysis.h
// For fitting residuals histograms with double gaus fit in ResPlots
struct DoubGausFitResult{
  FitResult fitResult = fUNKNOWNFIT;
  Double_t mean;
  Double_t meanError;
  Double_t amplitudeSignal;
  Double_t amplitudeSignalError;
  Double_t sigmaSignal;
  Double_t sigmaSignalError;
  Double_t amplitudeBackground;
  Double_t amplitudeBackgroundError;
  Double_t sigmaBackground;
  Double_t sigmaBackgroundError;
};

#endif

