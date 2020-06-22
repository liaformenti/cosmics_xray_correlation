#define ResPlots_cxx
#include "ResPlots.h"

using namespace std;

ResPlots::ResPlots(vector<Residual>* _residuals,
                   Binning* _binning,
                   DetectorGeometry* _g,
                   PlotManager* _pm) : residuals(_residuals),
                   binning(_binning), g(_g), pm(_pm) {}

// Note that bin edges vectors of binning are transfered to Double_t
// arrays for use with PlotManager methods.
void ResPlots::InitializeNumEntriesTH2Is(string nameBase) {
    string name;
    string title;
    Double_t xEdges[binning->nBinsX + 1];
    copy(binning->xBinEdges.begin(), binning->xBinEdges.end(), xEdges);
    Double_t yEdges[binning->nBinsY + 1];
    copy(binning->yBinEdges.begin(), binning->yBinEdges.end(), yEdges);
    vector<Combination> combVec = combinationVector();    
    for (auto combo=combVec.begin(); combo!=combVec.end(); combo++) {
        name = nameBase + combo->String();
        title = "Layer: " + to_string(combo->layer); 
        title += ", Fixed Layers: " + to_string(combo->fixed1);
        title += to_string(combo->fixed2);
        title += " No. Entries;x [mm];y [mm];";
        pm->Add(name, title, binning->nBinsX, xEdges,
               binning->nBinsY, yEdges, myTH2I); 
    }
    return;
}

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

// Names of residual Gaussians based on x and y position bin number
// Starts from lowest coord value with bin number 1 and increments
// in both directions.
void ResPlots::InitializePosBinnedResPlots(string nameBase) {
    string name;
    string title;
    // Will hold bin edges for title
    Float_t x, xp1;
    Float_t y, yp1;
    vector<Combination> comboVec = combinationVector();
    // Only go up to minus 1 as to not make plot starting from last bin
    // for (auto x=binning->xBinEdges.begin(); 
    // x!=binning->xBinEdges.end()-1; x++) {
    for (Int_t i=0; i<binning->xBinEdges.size()-1; i++) {
        // for (auto y=binning->yBinEdges.begin(); y!=binning->yBinEdges.end()-1; y++) {
        for (Int_t j=0; j<binning->yBinEdges.size()-1; j++) {
                 for(auto combo=comboVec.begin(); 
                 combo!=comboVec.end(); combo++){ 
                     x = binning->xBinEdges.at(i);
                     xp1 = binning->xBinEdges.at(i+1);
                     y = binning->yBinEdges.at(j);
                     yp1 = binning->yBinEdges.at(j+1);
                     // cout << x << ' ' << xp1 << ' ' << y << ' ' << yp1 << '\n';
                     // name = "residuals_x_in_" + to_string(*x) + "-";
                     // name += to_string(*x+1) + "_y_in_";
                     // name += to_string(*y);
                     // name += "-" + to_string(*y+1) + "_";
                     name = "residuals_xbin_" + to_string(i+1);
                     name += "_ybin_" + to_string(j+1) + "_";
                     name += combo->String();
                     title = "Layer: ";
                     title += to_string(combo->layer);
                     title +=", Fixed Layers: ";
                     title += to_string(combo->fixed1);
                     title += to_string(combo->fixed2);
	             // title += ", x#in["+Tools::CStr(*x,2)+",";
                     // title += Tools::CStr(*x+1,2)+"] mm, ";
	             // title += "y#in["+Tools::CStr(*y,2)+",";
                     // title += Tools::CStr(*y+1,2)+"] mm";
                     title += ", x#in["+Tools::CStr(x,2)+",";
                     title += Tools::CStr(xp1,2)+"] mm, ";
	             title += "y#in["+Tools::CStr(y,2)+",";
                     title += Tools::CStr(yp1,2)+"] mm";
	             title += ";Residuals [mm];Tracks";

                    // Guessed appropriate range and num bins
                    pm->Add(name, title, 100, -10, 10, myTH1I);
            }
        }
    }
    // TH2Fs for mean and stdev of each bin
    /*for(auto combo=comboVec.begin(); combo!=comboVec.end(); combo++) {
        name = 
    }*/
    return;
}
// Name base is for TH2s. For TH1, prefix is residuals_
void ResPlots::CreatePosBinnedResPlots(string nameBase) {
    InitializePosBinnedResPlots(nameBase);
    Combination combo;
    Int_t xbin, ybin; // bin numbers in x and y
    string name;
    for (auto r=residuals->begin(); r!=residuals->end(); r++) {
        combo = r->GetCombo();
        // Get bin numbers
        for (Int_t i=0; i<binning->xBinEdges.size()-1; i++) {
            if ( (binning->xBinEdges.at(i) < r->x) && 
                 (r->x < binning->xBinEdges.at(i+1)) ) {
                xbin = i+1;
                break;
            }
        }
        for (Int_t i=0; i<binning->yBinEdges.size()-1; i++) {
            if ( (binning->yBinEdges.at(i) < r->y) && 
                 (r->y < binning->yBinEdges.at(i+1)) ) {
                ybin = i+1;
                break;
            }
        }
        // Fill plots
       name = "residuals_xbin_" + to_string(xbin) + "_ybin_";
       name += to_string(ybin) + "_" + combo.String();
       pm->Fill(name, r->res);
    }
    /*TCanvas* c = new TCanvas();
    TH1I* hist = (TH1I*)pm->Get("residuals_xbin_1_ybin_1_layer1_fixedlayers34");
    hist->Draw();
    c->Print("resPlot.pdf");
    return;*/
}

void ResPlots::PrintPosBinnedResPlots(string nameBase, string filename){
    TCanvas* c = new TCanvas();
    c->Print((filename +"[").c_str());
    TH1I* hist; // temp var
    vector<Combination> comboVec = combinationVector();
    string name;
    for (auto combo=comboVec.begin(); combo!=comboVec.end(); combo++) {
        for (Int_t i=0; i<binning->xBinEdges.size()-1; i++) {
            for (Int_t j=0; j<binning->yBinEdges.size()-1; j++) {
                name = "residuals_xbin_" + to_string(i+1) + "_ybin_";
                name += to_string(j+1) + "_" + combo->String();
                hist = (TH1I*)pm->Get(name);
                if (hist->GetEntries() != 0) {
                    hist->Draw();
                    c->Print(filename.c_str());
                }
            }
        }
    }
    c->Print((filename + "]").c_str());
    delete c;
    return;
}
