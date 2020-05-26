#define StatsStudy_cxx
#include "StatsStudy.h"

using namespace std;

// To study bin sizes regardless of x-ray position

StatsStudy::StatsStudy(std::vector<Residual>* _residuals,
               DetectorGeometry* _g,
               PlotManager* _pm) : 
               residuals(_residuals), pm(_pm), g(_g) {
    /*cout << "constructed\n";
    cout << g->GetZPosition(4) << '\n';
    pm->PrintAll();
    for (auto r=residuals->begin(); r!=residuals->end(); r++) {
        cout << r->res << ' ';
    }*/
}
// Initializes histograms over layer area for each
// fixed-layer / residual permutation.
// *** Width in mm *** 
void StatsStudy::InitializeSquareBinHistograms(Double_t width) {
   pair<Double_t, Double_t> xlims = g->GetModuleLimitsX();
   pair<Double_t, Double_t> ylims = g->GetModuleLimitsY();
   Int_t nbinsx = (xlims.second - xlims.first)/width;
   Int_t nbinsy = (ylims.second - ylims.first)/width;
   string name;
   name = "Residual_layer3_fixedlayers12_width_" + 
          to_string(width) + "mm";
   // pm->Add(name, name,  
   return;
}

