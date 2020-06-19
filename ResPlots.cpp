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
// Name base is for TH2s. For TH1, prefix is residuals_
void ResPlots::CreatePosBinnedResPlots(string nameBase) {
    InitializePosBinnedResPlots(nameBase);
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

void ResPlots::InitializePosBinnedResPlots(string nameBase) {
    string name;
    string title;
    vector<Combination> comboVec = combinationVector();
    // Only go up to minus 1 as to not make plot starting from last bin
    for (auto x=binning->xBinEdges.begin(); 
    x!=binning->xBinEdges.end()-1; x++) {
        for (auto y=binning->yBinEdges.begin(); y!=binning->yBinEdges.end()-1; y++) {
                 for(auto combo=comboVec.begin(); 
                 combo!=comboVec.end(); combo++){ 
                     name = "residuals_x_in_" + to_string(*x) + "-";
                     name += to_string(*x+1) + "_y_in_";
                     name += to_string(*y);
                     name += "-" + to_string(*y+1) + "_";
                     name += combo->String();
                     title = "Layer: ";
                     title += to_string(combo->layer);
                     title +=", Fixed Layers:";
                     title += to_string(combo->fixed1);
                     title += to_string(combo->fixed2);
	             title += ", x#in["+Tools::CStr(*x,2)+",";
                     title += Tools::CStr(*x+1,2)+"] mm, ";
	             title += "y#in["+Tools::CStr(*y,2)+",";
                     title += Tools::CStr(*y+1,2)+"] mm";
	             title += ";Residuals [mm];Tracks";

                    // Guessed appropriate range and num bins
                    pm->Add(name, title, 100, -10, 10, myTH1I);
            }
        }
    }
    // Add TH2Fs
    // Start with just sigma for now.
    return;
}
