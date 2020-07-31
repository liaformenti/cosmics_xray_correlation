#define Helper_cxx
#include "Helper.h"

using namespace std;

string Combination::String() const {
    string s = "layer" + to_string(layer) + "_fixedlayers" + to_string(fixed1) + to_string(fixed2);
    return s;
}

vector<Combination> combinationVector() {
    vector<Combination> vComb;
    vComb.push_back(Combination(3, 1, 2));
    vComb.push_back(Combination(4, 1, 2));
    vComb.push_back(Combination(2, 1, 3));
    vComb.push_back(Combination(4, 1, 3));
    vComb.push_back(Combination(2, 1, 4));
    vComb.push_back(Combination(3, 1, 4));
    vComb.push_back(Combination(1, 2, 3));
    vComb.push_back(Combination(4, 2, 3));   
    vComb.push_back(Combination(1, 2, 4));
    vComb.push_back(Combination(3, 2, 4));
    vComb.push_back(Combination(1, 3, 4));
    vComb.push_back(Combination(2, 3, 4));
    return vComb;
}

// Input info constructor
// Based on quadname, fills wedge id with the ID string of the wedge
// that quad is a part of
InputInfo::InputInfo(string theQuadName, string theOutPath) :
                     quadname(theQuadName), outpath(theOutPath) {
    // Map where quadname string is key and value is MTF (wedge) string
    // Must be updated as wedges are assembled and
    // as you run over more quads
    // Currently only contains QS3Ps that have x-ray data in results.db
    unordered_map<string, string> quadToWedge; 
    quadToWedge.insert(pair<string, string>("QS3P06", 
                                            "20MNIWSAP00002"));
    quadToWedge.insert(pair<string, string>("QS3P10", 
                                            "20MNIWSAP00008"));
    // wedgeid = quadToWedge.at(quadname);
    if (quadToWedge.find(quadname) != quadToWedge.end()) 
        wedgeid = quadToWedge.at(quadname);
    else {
        cout << "Warning: quadruplet does not have x-ray data.\nIf it does, add the quadname and the MTF to the quadToWedge map in Helper.cpp (InputInfo constructor).\n\n";
        wedgeid = ""; // Empty string for no wedge
    }
}

void getOtherLayers(UShort_t la, UShort_t lb, UShort_t* lc, UShort_t* ld) {
    string throwStatement = "Invalid layer numbers: " + to_string(la);
    throwStatement += ", " + to_string(lb) + ", (getOtherLayers, ";
    throwStatement += "Helper.cpp).\n\n";
    switch(la) {
        case 1 : switch(lb) {
                     case 2 : *lc=3; *ld=4;
                              return;                        
                     case 3 : *lc=2; *ld=4;
                              return;
                     case 4 : *lc=2; *ld=3;
                              return;
                     default : throw invalid_argument(throwStatement);
                  }
        case 2 : switch(lb) {
                     case 3 : *lc=1; *ld=4;
                              return;
                     case 4 : *lc=1; *ld=3;
                              return;
                     default : throw invalid_argument(throwStatement);

                 }
        case 3 : switch(lb) {
                     case 4 : *lc=1; *ld=2;
                              return;
                     default : throw invalid_argument(throwStatement);
                 }
        default : throw invalid_argument(throwStatement);
    }
    return;
}
// string getCombinationPlotName(string header, string footer, UShort_t layer, UShort_t
