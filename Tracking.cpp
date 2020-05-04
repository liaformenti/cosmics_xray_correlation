// Functions to build tracks from hits on 2 layers
#define RunAnalysis_cxx
#include "Tracking.h"

using namespace std;

// De-pluralize
Tracking::Tracking(DetectorGeometry* _g, 
                   map<UShort_t, Double_t> hitsMapX,
                   map<UShort_t, Double_t>* tracksMapX,
                   map<UShort_t, Double_t> hitsMapY,
                   map<UShort_t, Double_t>* tracksMapY,
                   UShort_t fixedLayer1, UShort_t fixedLayer2)
                   : g(_g){
    // g = g;
    hitsX = hitsMapX;
    trackX = tracksMapX;
    hitsY = hitsMapY;
    trackY = tracksMapY;
    la = fixedLayer1;
    lb = fixedLayer2;
    cout << "Tracking\n";
    cout << "  x hits " << hitsX[la] << ' ' <<  hitsX[lb] << '\n';
    cout << "  y hits " << hitsY[la] << ' ' << hitsY[lb] << '\n';
    cout << "  z pos " << g->GetZPosition(la) << ' ' << g->GetZPosition(lb) << '\n';
    // trackX[1] = 3.14;
    trackX->insert(pair<UShort_t, Double_t>(1, 3.14));
    cout << "  set trackx" << trackX->find(1)->second << '\n';

} 

void Tracking::Track() {
    cout << "Track\n";
    cout << "  Layers " << la << ' ' << lb << '\n';
    cout << "  x hits " << hitsX[la] << ' ' << hitsX[lb] << '\n';
    cout << "  y hits " << hitsY[la] << ' ' << hitsY[lb] << '\n';
    cout << "  z pos " << g->GetZPosition(la) << ' ' << g->GetZPosition(lb) << '\n';
    trackX->find(1)->second = 2.7;
    cout << "  set trackx" << trackX->find(1)->second << '\n';
}
