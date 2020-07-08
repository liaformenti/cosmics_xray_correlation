#ifndef Helper_h
#define Helper_h

// Useful functions not belonging to any clas

// C++ includes
#include <iostream>
#include <string>

// Root includes
#include <TROOT.h>

// Data info contains the quadruplet name (eg. qs3p7) and
// the operating voltage (eg. 2900V), and the wedge id (eg. WSAP00002)
struct DataInfo { 
  std::string quadname;
  std::string wedgeid;
  std::string otherInfo = "";
};

// NOW MAKE HELPER THAT TAKES IN DATAINFO WITH QUAD NAME AND FILLS
// WEDGEID

struct Combination {
  Combination(){};
  Combination(UShort_t theLayer, UShort_t firstFixedLayer, UShort_t secondFixedLayer) : layer(theLayer), fixed1(firstFixedLayer), fixed2(secondFixedLayer){};
  UShort_t layer;
  UShort_t fixed1;
  UShort_t fixed2;
  std::string String() const;
};

std::vector<Combination> combinationVector(); 

void getOtherLayers(UShort_t la, UShort_t lb, UShort_t* lc, UShort_t* ld);
// for creating strings of the form:
// header_layerC_fixedLayersAB_footer
// string getPermPlotName(string header, string footer, UShort_t layer, UShort_t fixedLayer1, UShort_t fixedLayer2);
#endif
