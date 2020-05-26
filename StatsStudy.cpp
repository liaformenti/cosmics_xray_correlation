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
void StatsStudy::InitializeSquareBinHistograms(Int_t width) {
   binWidth = width;
   pair<Double_t, Double_t> xlims = g->GetModuleLimitsX();
   pair<Double_t, Double_t> ylims = g->GetModuleLimitsY();
   Int_t nbinsx = floor((xlims.second - xlims.first)/width);
   Int_t nbinsy = floor((ylims.second - ylims.first)/width);
   string name;
   name = "Residual_layer3_fixedlayers12_width_" + 
          to_string(width) + "mm";
   pm->Add(name, name, nbinsx, xlims.first, xlims.second,
                       nbinsy, ylims.first, ylims.second, myTH2F); 
   name = "Residual_layer4_fixedlayers12_width_" + 
          to_string(width) + "mm";
   pm->Add(name, name, nbinsx, xlims.first, xlims.second,
                       nbinsy, ylims.first, ylims.second, myTH2F); 
   name = "Residual_layer2_fixedlayers13_width_" + 
          to_string(width) + "mm";
   pm->Add(name, name, nbinsx, xlims.first, xlims.second,
                       nbinsy, ylims.first, ylims.second, myTH2F); 
   name = "Residual_layer4_fixedlayers13_width_" + 
          to_string(width) + "mm";
   pm->Add(name, name, nbinsx, xlims.first, xlims.second,
                       nbinsy, ylims.first, ylims.second, myTH2F); 
   name = "Residual_layer2_fixedlayers14_width_" + 
          to_string(width) + "mm";
   pm->Add(name, name, nbinsx, xlims.first, xlims.second,
                       nbinsy, ylims.first, ylims.second, myTH2F); 
   name = "Residual_layer3_fixedlayers14_width_" + 
          to_string(width) + "mm";
   pm->Add(name, name, nbinsx, xlims.first, xlims.second,
                       nbinsy, ylims.first, ylims.second, myTH2F); 
   name = "Residual_layer1_fixedlayers23_width_" + 
          to_string(width) + "mm";
   pm->Add(name, name, nbinsx, xlims.first, xlims.second,
                       nbinsy, ylims.first, ylims.second, myTH2F); 
   name = "Residual_layer4_fixedlayers23_width_" + 
          to_string(width) + "mm";
   pm->Add(name, name, nbinsx, xlims.first, xlims.second,
                       nbinsy, ylims.first, ylims.second, myTH2F); 
   name = "Residual_layer1_fixedlayers24_width_" + 
          to_string(width) + "mm";
   pm->Add(name, name, nbinsx, xlims.first, xlims.second,
                       nbinsy, ylims.first, ylims.second, myTH2F); 
   name = "Residual_layer3_fixedlayers24_width_" + 
          to_string(width) + "mm";
   pm->Add(name, name, nbinsx, xlims.first, xlims.second,
                       nbinsy, ylims.first, ylims.second, myTH2F); 
   name = "Residual_layer1_fixedlayers34_width_" + 
          to_string(width) + "mm";
   pm->Add(name, name, nbinsx, xlims.first, xlims.second,
                       nbinsy, ylims.first, ylims.second, myTH2F); 
   name = "Residual_layer2_fixedlayers34_width_" + 
          to_string(width) + "mm";
   pm->Add(name, name, nbinsx, xlims.first, xlims.second,
                       nbinsy, ylims.first, ylims.second, myTH2F); 
   return;
}

// Loops through residuals and puts them in appropriate bins and hists
void StatsStudy::FillSquareBinHistograms() {
    string plotName;
    for (auto r=residuals->begin(); r!=residuals->end(); r++) {
        plotName = "Residual_layer" + to_string(r->l) + "_fixedlayers" + to_string(r->la) + to_string(r->lb) + "_width_" + "mm"; 
        cout << plotName << '\n';
    }
    return;
}
