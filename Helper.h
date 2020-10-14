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

// A "Combination" is 2 fixed layers and the layer of interest
struct Combination {
  Combination(){};
  Combination(UShort_t theLayer, UShort_t firstFixedLayer, UShort_t secondFixedLayer) : layer(theLayer), fixed1(firstFixedLayer), fixed2(secondFixedLayer){};
  UShort_t layer;
  UShort_t fixed1;
  UShort_t fixed2;
  std::string String() const;
};

std::vector<Combination> combinationVector(); 

// Data info contains the quadruplet name (eg. QS3P07),
// the wedge id (eg. WSAP00002), and the output directory path.
// The output directory must exist and its name should likely include
// the quadname and the run voltage of the input datafile. 
struct InputInfo { 
  InputInfo(){};
  InputInfo(std::string theQuadName, std::string theOutPath, std::string theTag);
  std::string quadname;
  std::string wedgeid;
  std::string outpath;
  std::string tag; // used to name files
};

std::string quadToWedge(InputInfo* info);

void getOtherLayers(UShort_t la, UShort_t lb, UShort_t* lc, UShort_t* ld);
// for creating strings of the form:
// header_layerC_fixedLayersAB_footer
// string getPermPlotName(string header, string footer, UShort_t layer, UShort_t fixedLayer1, UShort_t fixedLayer2);
#endif
