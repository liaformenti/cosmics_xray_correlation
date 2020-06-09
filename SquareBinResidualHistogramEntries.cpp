#define SquareBinResidualHistogramEntries_cxx
#include "SquareBinResidualHistogramEntries.h"

using namespace std;

// To study bin sizes regardless of x-ray position

SquareBinResidualHistogramEntries::SquareBinResidualHistogramEntries(std::vector<Residual>* _residuals,
               DetectorGeometry* _g,
               PlotManager* _pm) : 
               residuals(_residuals), g(_g), pm(_pm) {
}
// Initializes histograms over layer area for each
// fixed-layer / residual permutation.
// *** Width in mm *** 
void SquareBinResidualHistogramEntries::InitializeSquareBinHistograms(Int_t width) {
   binWidth = width;
   pair<Double_t, Double_t> xlims = g->GetModuleLimitsX();
   pair<Double_t, Double_t> ylims = g->GetModuleLimitsY();
   // Tradiational binning method
   // Int_t nbinsx = floor((xlims.second - xlims.first)/width);
   // Int_t nbinsy = floor((ylims.second - ylims.first)/width);
   // Variable binning method
   Int_t nbinsx = ceil((xlims.second - xlims.first)/binWidth);
   Int_t nbinsy = ceil((ylims.second - ylims.first)/binWidth);
   Double_t xBins[nbinsx + 1];
   Double_t yBins[nbinsy + 1]; 
   // cout << xlims.first << ' ' << xlims.second << ' ' << nbinsx << '\n';
   // highest x bin width not controlled
   for (Int_t i=0; i<nbinsx; i++) {
       xBins[i] = xlims.first + i*binWidth;
       // cout << xBins[i] << ' ';
   }
   xBins[nbinsx] = xlims.second;
   // cout << xBins[nbinsx] << '\n';
   // cout << ylims.first << ' ' << ylims.second << ' ' << nbinsy << ' ' << nbinsy*binWidth << '\n';
   // highest y bin width not controlled
   /*for (Int_t i=0; i<nbinsy; i++) {
       yBins[i] = ylims.first + i*binWidth;
       // cout << yBins[i] << ' ';
   }
   yBins[nbinsy] = ylims.second;*/
   // lowest y bin width not controlled
   // cout << yBins[nbinsy] << '\n';
   for (Int_t i=0; i<nbinsy; i++) {
       yBins[nbinsy - i] = ylims.second - i*binWidth;
       // cout << yBins[nbinsy - i] << ' ';
   }
   yBins[0] = ylims.first;
   // cout << yBins[0] << '\n';
   string name;
   vector<Combination> combVec = combinationVector();
   for (auto combo=combVec.begin(); combo!=combVec.end(); combo++) {
       name = "Residual_" + combo->String() + "_width_" + to_string(binWidth) + "mm";
        // pm->Add(name, name, nbinsx, xlims.first, xlims.second,
        //        nbinsy, ylims.first, ylims.second, myTH2F); 
        pm->Add(name, name, nbinsx, xBins, nbinsy, yBins, myTH2F);
   }
   return;
}

// Loops through residuals and puts them in appropriate bins and hists
void SquareBinResidualHistogramEntries::FillSquareBinHistograms() {
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

string SquareBinResidualHistogramEntries::GetSquareBinHistName(UShort_t layer, 
             UShort_t fixedLayer1, UShort_t fixedLayer2) {
    string name = "Residual_layer" + to_string(layer) +
                  "_fixedlayers" + to_string(fixedLayer1) +
                  to_string(fixedLayer2) + "_width_" +
                  to_string(binWidth) + "mm";
    return name; 
}

void SquareBinResidualHistogramEntries::PrintSquareBinHistograms(string filename) {
    TCanvas* c = new TCanvas();
    c->Print((filename + "[").c_str());
    UShort_t lc; UShort_t ld;
    TH2F* hist; // temp var
    for (UShort_t la=1; la<=4; la++) {
        for (UShort_t lb=(la+1); lb<=4; lb++) {
            getOtherLayers(la, lb, &lc, &ld);
            hist = (TH2F*)pm->Get(GetSquareBinHistName(lc, la, lb));
            if (hist->GetEntries() != 0) {
                hist->Draw("Colz");
                c->Print(filename.c_str());
                c->Clear();
            }
            hist = (TH2F*)pm->Get(GetSquareBinHistName(ld, la, lb));
            if (hist->GetEntries() != 0) {
                hist->Draw("Colz");
                c->Print(filename.c_str()); 
                c->Clear();
            }
        }

    }
    c->Print((filename + "]").c_str());
    delete c;
    return;
} 
