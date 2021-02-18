#define ResPlots_cxx
#include "ResPlots.h"

using namespace std;

ResPlots::ResPlots(vector<Residual>* _residuals,
                   Binning* _binning,
                   std::string namebase,
                   AnalysisInfo* _cinfo,
                   DetectorGeometry* _g,
                   PlotManager* _pm,
                   InputInfo* _myInfo) : residuals(_residuals),
                   binning(_binning), cinfo(_cinfo), g(_g), pm(_pm),
                   myInfo(_myInfo) {
    // Initialize nameBase
    nameBase = namebase;
}

// Note that bin edges vectors of binning are transfered to Double_t
// arrays for use with PlotManager methods.
void ResPlots::InitializeNumEntriesTH2Is() {
    string name;
    string title;
    Double_t xEdges[binning->nBinsX + 1];
    copy(binning->xBinEdges.begin(), binning->xBinEdges.end(), xEdges);
    Double_t yEdges[binning->nBinsY + 1];
    copy(binning->yBinEdges.begin(), binning->yBinEdges.end(), yEdges);
    vector<Combination> combVec = combinationVector();    
    for (auto combo=combVec.begin(); combo!=combVec.end(); combo++) {
        name = nameBase + "_num_entries_" + combo->String();
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
void ResPlots::CreateNumEntriesTH2Is() {
    InitializeNumEntriesTH2Is();
    string name;
    Combination combo;
    for (auto r=residuals->begin(); r!=residuals->end(); r++) {
        combo = r->GetCombo(); 
        name = nameBase + "_num_entries_" + combo.String();
        pm->Fill(name, r->x, r->y);
    }
    return;
}

void ResPlots::PrintNumEntriesTH2Is(string filename) {
    TCanvas* c = new TCanvas();
    c->Print((filename + "[").c_str());
    TH2I* hist; // temp var
    vector<Combination> comboVec = combinationVector();
    for (auto combo=comboVec.begin(); combo!=comboVec.end(); combo++) {
        // NEED TO ADD UNIQUE IDENTIFIER TO THIS AS WELL (SAME AS ^)
        hist = (TH2I*)pm->Get(nameBase + "_num_entries_" + combo->String());  
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

// Names of position binned residual plots based on x and y position 
// bin number. Starts from lowest coord value with bin number 1 and increments in both directions.
void ResPlots::InitializePosBinnedResPlots() {
    string name;
    string title;
    // Will hold bin edges for title
    Float_t x, xp1;
    Float_t y, yp1;
    vector<Combination> comboVec = combinationVector();
    // Only go up to minus 1 as to not make plot starting from last bin
    for (Int_t i=0; i<binning->xBinEdges.size()-1; i++) {
        for (Int_t j=0; j<binning->yBinEdges.size()-1; j++) {
                 for (auto combo=comboVec.begin(); 
                 combo!=comboVec.end(); combo++) { 
                     x = binning->xBinEdges.at(i);
                     xp1 = binning->xBinEdges.at(i+1);
                     y = binning->yBinEdges.at(j);
                     yp1 = binning->yBinEdges.at(j+1);
                     name = nameBase;
                     name += "_residuals_xbin_" + to_string(i+1);
                     name += "_ybin_" + to_string(j+1) + "_";
                     name += combo->String();
                     title = "Layer: ";
                     title += to_string(combo->layer);
                     title +=", Fixed Layers: ";
                     title += to_string(combo->fixed1);
                     title += to_string(combo->fixed2);
                     title += ", x#in["+Tools::CStr(x,2)+",";
                     title += Tools::CStr(xp1,2)+"] mm, ";
	             title += "y#in["+Tools::CStr(y,2)+",";
                     title += Tools::CStr(yp1,2)+"] mm";
	             title += ";Residuals [mm];Tracks";

                    // Width: 0.1 mm, range to do with usual spread
                    pm->Add(name, title, 200, -10, 10, myTH1I);
            }
        }
    }
    // TH2Fs for mean and stdev of each bin
    /*for(auto combo=comboVec.begin(); combo!=comboVec.end(); combo++) {
        name = 
    }*/
    return;
}

void ResPlots::CreatePosBinnedResPlots() {
    InitializePosBinnedResPlots();
    // Fill plots
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
       name = nameBase + "_residuals_xbin_" + to_string(xbin);
       name += "_ybin_" + to_string(ybin) + "_" + combo.String();
       pm->Fill(name, r->res);
    }
} // end

void ResPlots::PrintPosBinnedResPlots(string filename){
    TCanvas* c = new TCanvas();
    c->Divide(2,2); // 4 plots per pdf page
    c->Print((filename +"[").c_str());
    TH1I* hist; // temp var
    vector<Combination> comboVec = combinationVector();
    string name;
    Int_t count = 0;
    for (auto combo=comboVec.begin(); combo!=comboVec.end(); combo++) {
        for (Int_t i=0; i<binning->xBinEdges.size()-1; i++) {
            for (Int_t j=0; j<binning->yBinEdges.size()-1; j++) {
                name = nameBase + "_residuals_xbin_" + to_string(i+1);
                name += "_ybin_" + to_string(j+1) + "_"; 
                name += combo->String();
                hist = (TH1I*)pm->Get(name);
                if (hist->GetEntries() != 0) {
                    c->cd(count%4 + 1);
                    hist->Draw();
                    if (count%4==3) { // Add page once canvas is full
                        c->Print(filename.c_str());
                    }
                    count++;
                }
            }
        }
    }
    c->Print((filename + "]").c_str());
    delete c;
    return;
}

void ResPlots::InitializePosBinnedFitResultTH2Fs() {
    string name;
    string title;
    // Put bin vectors into arrays
    Double_t xEdges[binning->nBinsX + 1];
    copy(binning->xBinEdges.begin(), binning->xBinEdges.end(), xEdges);
    Double_t yEdges[binning->nBinsY + 1];
    copy(binning->yBinEdges.begin(), binning->yBinEdges.end(), yEdges);
    // Init plots
    vector<Combination> combVec = combinationVector();    
    vector<string> types = {"means", "sigmas"};
    for (auto combo=combVec.begin(); combo!=combVec.end(); combo++) {
        for (auto type=types.begin(); type!=types.end(); type++) {
            name = nameBase + "_" + *type + "_" + combo->String();
            title = "Layer: " + to_string(combo->layer); 
            title += ", Fixed Layers: " + to_string(combo->fixed1);
            title += to_string(combo->fixed2);
            title += " " + *type + " by position;x [mm];y [mm];";
            pm->Add(name, title, binning->nBinsX, xEdges,
                   binning->nBinsY, yEdges, myTH2F); 
        }
    }
    return;

}

// Note: You can also add fits to plot manager so that you could do
// fit in CreatePosBinnedResPlots, but access it here explicitly
// instead of doing it here
void ResPlots::CreatePosBinnedFitResultTH2Fs() {
    // should be in config
    Float_t invalidNum = -100;
    Float_t invalidErr = 0;
    InitializePosBinnedFitResultTH2Fs();
    vector<Combination> comboVec = combinationVector();
    string name;
    TH1I* hist;
    Double_t histMean, histRMS;
    Int_t status; // hold fit result
    TF1* theFit;
    Double_t mean, meanErr;
    Double_t sigma, sigmaErr;
    TH2F* th2F; 
    for (auto combo=comboVec.begin(); combo!=comboVec.end(); combo++){
        for (Int_t i=0; i<binning->xBinEdges.size()-1; i++) {
            for (Int_t j=0; j<binning->yBinEdges.size()-1; j++) {
                // For each bin and combination do fit
                // ****** Want to be able to select fit
                name = nameBase + "_residuals_xbin_" + to_string(i+1);
                name += "_ybin_" + to_string(j+1) + "_"; 
                name += combo->String();
                hist = (TH1I*)pm->Get(name);
                // If residual histogram has less that a min no. entries,
                // Fill bin with invalid value
                // Should be in config
                if (hist->GetEntries() < 50) {
                    name = nameBase + "_means_" + combo->String();
                    th2F = (TH2F*)pm->GetTH2F(name);
                    th2F->SetBinContent(i+1, j+1, invalidNum);
                    th2F->SetBinError(i+1, j+1, invalidErr);
                    name = nameBase + "_sigmas_" + combo->String();
                    th2F = (TH2F*)pm->GetTH2F(name);
                    th2F->SetBinContent(i+1, j+1, invalidNum);
                    th2F->SetBinError(i+1, j+1, invalidErr);
                    continue;
                }
                // Range of fit is desired fit range
                // Based on histogram RMS
                // Should be in config
                histMean = hist->GetMean();
                histRMS = hist->GetRMS();
                TF1* fit = new TF1("myGaus", "gaus", histMean - histRMS, histMean + histRMS);
                fit->SetParameter(0, 100); // Guess for amplitude
                fit->SetParameter(1, histMean); // Guess for mean
                fit->SetParameter(2, histRMS); // Guess for sigma
                status = hist->Fit("myGaus", "SQRL");
                if (status==0) { // Fit was success, fill TH2Fs
                    theFit = (TF1*)hist->GetFunction("myGaus");     
                    mean = theFit->GetParameter(1); 
                    meanErr = theFit->GetParError(1);
                    sigma = theFit->GetParameter(2);
                    sigmaErr = theFit->GetParError(2);
                    // Now get TH2Fs for mean and sigma
                    name = nameBase + "_means_" + combo->String();
                    th2F = (TH2F*)pm->GetTH2F(name);
                    th2F->SetBinContent(i+1, j+1, mean);
                    th2F->SetBinError(i+1, j+1, meanErr);
                    name = nameBase + "_sigmas_" + combo->String();
                    th2F = (TH2F*)pm->GetTH2F(name);
                    th2F->SetBinContent(i+1, j+1, sigma);
                    th2F->SetBinError(i+1, j+1, sigmaErr);
                }
                else { // if the fit fails, fill bin with invalid
                    name = nameBase + "_means_" + combo->String();
                    th2F = (TH2F*)pm->GetTH2F(name);
                    th2F->SetBinContent(i+1, j+1, invalidNum);
                    th2F->SetBinError(i+1, j+1, invalidErr);
                    name = nameBase + "_sigmas_" + combo->String();
                    th2F = (TH2F*)pm->GetTH2F(name);
                    th2F->SetBinContent(i+1, j+1, invalidNum);
                    th2F->SetBinError(i+1, j+1, invalidErr);
                }
            } // end y bin loop
        } // end x bin loop
    }  // end combo loop
}

void ResPlots:: PrintPosBinnedFitResultTH2Fs(string filename) {
    TCanvas* c = new TCanvas();
    c->Print((filename + "[").c_str());
    TH2F* hist; // temp var
    vector<Combination> comboVec = combinationVector();
    for (auto combo=comboVec.begin(); combo!=comboVec.end(); combo++) {
        // Means
        hist = (TH2F*)pm->Get(nameBase + "_means_" + combo->String());  
        if (hist->GetEntries() != 0) { // If plot is not empty,
            // These max and min values should go in config
            hist->SetMaximum(0.5);
            hist->SetMinimum(-0.5);
            hist->Draw("Colz");
            c->Print(filename.c_str());
            c->Clear();
        }
        // Stdevs
        hist = (TH2F*)pm->Get(nameBase + "_sigmas_" + combo->String()); 
        if (hist->GetEntries() != 0) {
            // These max and min values should go in config
            hist->SetMaximum(0.5);
            hist->SetMinimum(0);
            hist->Draw("Colz");
            c->Print(filename.c_str());
            c->Clear();
        }

    }
    c->Print((filename + "]").c_str());
    delete c;
    return;
}
