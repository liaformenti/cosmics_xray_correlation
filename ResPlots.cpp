#define ResPlots_cxx
#include "ResPlots.h"

using namespace std;

ResPlots::ResPlots(vector<Residual>* _residuals,
                   Binning* _binning,
                   DetectorGeometry* _g,
                   PlotManager* _pm) : residuals(_residuals),
                   binning(_binning), g(_g), pm(_pm) {}

// Shows number of entries in each bin
// Plot for each combination
void ResPlots::CreateNumEntriesTH2Is(string nameBase) {
    InitializeNumEntriesTH2Is(nameBase);
    string name;
    Combination combo;
    for (auto r=residuals->begin(); r!=residuals->end(); r++) {
        combo = r->GetCombo(); 
        name = nameBase + combo.String();
        pm->Fill(name, r->x, r->y);
    }
    /*Combination c1(1,3,4);
    Combination c2(2,3,4);
    TCanvas* c = new TCanvas("canvas", "canvas");
    c->Print("test.pdf[");
    TH2F* l1Hist = (TH2F*) pm->Get(nameBase+c1.String());
    l1Hist->Draw("Colz");
    c->Print("test.pdf");
    c->Clear();
    TH2F* l2Hist = (TH2F*) pm->Get(nameBase+c2.String());
    l2Hist->Draw("Colz");
    c->Print("test.pdf");
    c->Print("test.pdf]");
    delete c;*/
    return;
}

// Note that bin edges vectors of binning are transfered to Double_t
// arrays for use with PlotManager methods.
void ResPlots::InitializeNumEntriesTH2Is(string nameBase) {
    string name;
    Double_t xEdges[binning->nBinsX + 1];
    copy(binning->xBinEdges.begin(), binning->xBinEdges.end(), xEdges);
    Double_t yEdges[binning->nBinsY + 1];
    copy(binning->yBinEdges.begin(), binning->yBinEdges.end(), yEdges);
    vector<Combination> combVec = combinationVector();    
    for (auto combo=combVec.begin(); combo!=combVec.end(); combo++) {
        name = nameBase + combo->String();
        pm->Add(name, name, binning->nBinsX, xEdges,
               binning->nBinsY, yEdges, myTH2I); 
    }
    return;
}
