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
    return;
}

void ResPlots::PrintNumEntriesTH2Is(string nameBase, string filename) {
    TCanvas* c = new TCanvas();
    c->Print((filename + "[").c_str());
    TH2I* hist; // temp var
    vector<Combination> comboVec = combinationVector();
    for (auto combo=comboVec.begin(); combo!=comboVec.end(); combo++) {
        hist = (TH2I*)pm->Get(nameBase + combo->String());  
        if (hist->GetEntries() != 0) {
            hist->Draw("Colz");
            c->Print(filename.c_str());
            c->Clear();
        }
    }
    c->Print((filename + "]").c_str());
    delete c;
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
