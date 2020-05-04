// Functions to build tracks from hits on 2 layers
#define RunAnalysis_cxx
#include "Tracking.h"

using namespace std;

// De-pluralize
Tracking::Tracking(DetectorGeometry &g, 
                   map<UShort_t, Double_t> &hitsMapX,
                   map<UShort_t, Double_t> &tracksMapX,
                   map<UShort_t, Double_t> &hitsMapY,
                   map<UShort_t, Double_t> &tracksMapY,
                   UShort_t fixedL1, UShort_t fixedL2) {
    cout << "called\n";
    cout << hitsMapX[fixedL1] << hitsMapX[fixedL2] << '\n';
    cout << hitsMapY[fixedL1] << hitsMapY[fixedL2] << '\n';
    cout << g.GetZPosition(fixedL1) << g.GetZPosition(fixedL2) << '\n'; 
}
