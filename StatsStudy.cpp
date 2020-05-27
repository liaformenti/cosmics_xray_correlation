#define StatsStudy_cxx
#include "StatsStudy.h"

using namespace std;

// To study bin sizes regardless of x-ray position

StatsStudy::StatsStudy(std::vector<Residual>* _residuals,
               DetectorGeometry* _g,
               PlotManager* _pm) : 
               residuals(_residuals), g(_g), pm(_pm) {
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
        plotName = GetSquareBinHistName(r->l, r->la, r->lb);
        pm->Fill(plotName, r->x, r->y);
    }

    /*TCanvas* c = new TCanvas("canvas", "canvas");
    TH2F* l1Hist = (TH2F*) pm->Get(GetSquareBinHistName(1, 3, 4));
    l1Hist->Draw("Colz");
    c->Print((GetSquareBinHistName(1, 3, 4) + ".pdf").c_str());
    c->Clear();
    TH2F* l2Hist = (TH2F*) pm->Get(GetSquareBinHistName(2, 3, 4));
    l2Hist->Draw("Colz");
    c->Print((GetSquareBinHistName(2, 3, 4) + ".pdf").c_str());
    delete c;*/
    return;
}

string StatsStudy::GetSquareBinHistName(UShort_t layer, 
             UShort_t fixedLayer1, UShort_t fixedLayer2) {
    string name = "Residual_layer" + to_string(layer) +
                  "_fixedlayers" + to_string(fixedLayer1) +
                  to_string(fixedLayer2) + "_width_" +
                  to_string(binWidth) + "mm";
    return name; 
}

void PrintSquareBinHistograms(string filename) {
    return;
} 
