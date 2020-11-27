#define Retracking_cxx
#include "Retracking.h"

using namespace std;

Bool_t Retracking::MissingHitsOnFixedLayers(map<UShort_t, Double_t> &xTrack, 
                                            map<UShort_t, Double_t> &yTrack) {
    // If one or more of the fixed layers is missing a hit in x or y,
    // return true, else return false
    Bool_t missingHit =  ( ! ( (xTrack.find(la) != xTrack.end()) && 
                               (xTrack.find(lb) != xTrack.end()) && 
                               (yTrack.find(la) != yTrack.end()) && 
                               (yTrack.find(lb) != yTrack.end()) ) );
    return missingHit;
}
