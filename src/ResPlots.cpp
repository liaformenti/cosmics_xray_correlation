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
                     title = "#splitline{Layer: ";
                     title += to_string(combo->layer);
                     title +=", Fixed Layers: ";
                     title += to_string(combo->fixed1);
                     title += to_string(combo->fixed2);
                     title += "}{x#in["+Tools::CStr(x,2)+",";
                     title += Tools::CStr(xp1,2)+"] mm, ";
   	                 title += "y#in["+Tools::CStr(y,2)+",";
                     title += Tools::CStr(yp1,2)+"] mm}";
	                 title += ";Residuals [mm];Tracks";

                    // Width: 0.2 mm, range to do with usual spread
                    pm->Add(name, title, 60, -6, 6, myTH1I);
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
    gStyle->SetTitleSize(0.025, "t");
    gROOT->ForceStyle();
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
    gStyle->SetTitleSize(0.04, "t"); // reset title size
    gROOT->ForceStyle();
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

void ResPlots:: PrintPosBinnedFitResultTH2Fs() {
    // gStyle->SetOptFit(0);
    gStyle->SetOptStat(0);
    gROOT->ForceStyle();
    TCanvas* c = new TCanvas();
    TH2F* hist; // temp var
    vector<Combination> comboVec = combinationVector();
    string filename;

    // Means
    filename = myInfo->outpath + myInfo->tag + myInfo->quadname + "_fit_means_binning_" + binning->name + ".pdf";
    c->Print((filename + "[").c_str());
    for (auto combo=comboVec.begin(); combo!=comboVec.end(); combo++) {
        hist = (TH2F*)pm->Get(nameBase + "_means_" + combo->String());  
        if (hist->GetEntries() != 0) { // If plot is not empty,
            // These max and min values should go in config
            hist->SetMaximum(1);
            hist->SetMinimum(-1);
            hist->Draw("Colz");
            c->Print(filename.c_str());
            c->Clear();
        }
    }
    c->Print((filename + "]").c_str());

    // Sigmas
    filename = myInfo->outpath + myInfo->tag + myInfo->quadname + "_fit_sigmas_binning_" + binning->name + ".pdf";
    c->Print((filename + "[").c_str());
    for (auto combo=comboVec.begin(); combo!=comboVec.end(); combo++) {
        hist = (TH2F*)pm->Get(nameBase + "_sigmas_" + combo->String()); 
        if (hist->GetEntries() != 0) {
            // These max and min values should go in config
            hist->SetMaximum(1);
            hist->SetMinimum(0);
            hist->Draw("Colz");
            c->Print(filename.c_str());
            c->Clear();
        }

    }
    c->Print((filename + "]").c_str());
    delete c;
    // Return to regular style
    gStyle->SetOptStat("e");
    gROOT->ForceStyle();
    return;
}

void ResPlots::InitializeResidualDistributions() {
    string name, title;
    vector<Combination> combVec = combinationVector();    
    for (auto combo=combVec.begin(); combo!=combVec.end(); combo++) {
        name = nameBase + "_residual_distribution_" + combo->String();
        title = "Layer: " + to_string(combo->layer); 
        title += ", Fixed Layers: " + to_string(combo->fixed1);
        title += to_string(combo->fixed2);
        title += ";Residual [mm];No. Residuals";
        pm->Add(name, title, 60, -6, 6, myTH1F);
    }
    return;
}

void ResPlots::CreateResidualDistributions() {
    InitializeResidualDistributions();
    string name;
    Combination combo;
    for (auto r=residuals->begin(); r!=residuals->end(); r++) {
        combo = r->GetCombo(); 
        name = nameBase + "_residual_distribution_" + combo.String();
        pm->Fill(name, r->res);
    }
    return;
}

void ResPlots::PrintResidualDistributions(string filename) {
    gStyle->SetOptStat("emr");
    gROOT->ForceStyle();
    TCanvas* c = new TCanvas();
    c->Print((filename + "[").c_str());
    TH1F* hist; // temp var
    vector<Combination> comboVec = combinationVector();
    for (auto combo=comboVec.begin(); combo!=comboVec.end(); combo++) {
        // NEED TO ADD UNIQUE IDENTIFIER TO THIS AS WELL (SAME AS ^)
        hist = (TH1F*)pm->Get(nameBase + "_residual_distribution_" + combo->String());  
        if (hist->GetEntries() != 0) {
            hist->Draw();
            c->Print(filename.c_str());
            c->Clear();
        }
    }
    c->Print((filename + "]").c_str());
    delete c;
    gStyle->SetOptStat("e");
    gROOT->ForceStyle();
    return;
}

void ResPlots::InitializeDNLPlots() {
    // All residuals
    pm->Add(nameBase + "_residual_vs_yrel", ";y_{rel,hit};Residual [mm]",
            20, -0.5, 0.5, 40, -4, 4, myTH2F);
    // Residuals by combination
    string name, title;
    vector<Combination> combVec = combinationVector();    
    for (auto combo=combVec.begin(); combo!=combVec.end(); combo++) {
        name = nameBase + "_residual_vs_yrel_" + combo->String();
        title = "Layer: " + to_string(combo->layer); 
        title += ", Fixed Layers: " + to_string(combo->fixed1);
        title += to_string(combo->fixed2);
        title += ";y_{rel, hit};Residual [mm]";
        pm->Add(name, title, 20, -0.5, 0.5, 40, -4, 4, myTH2F);
    }

    return;
}

void ResPlots::CreateDNLPlots() {
    InitializeDNLPlots();
    string name;
    Combination combo;
    Double_t yrel;
    for (auto r=residuals->begin(); r!=residuals->end(); r++) {
        combo = r->GetCombo(); 
        yrel = CalculateYRelFromResidual(r->l, r->res, r->y, g);
        name = nameBase + "_residual_vs_yrel";
        // Fill all residuals DNL plot
        pm->Fill(name, yrel, r->res);
        name += "_" + combo.String();
        // Fill combo specific DNL plot
        pm->Fill(name, yrel, r->res);
    }
    // Now make profiles
    // First, all residuals
    MakeProfileX(nameBase + "_residual_vs_yrel", "profile");
    // Then, by combination
    vector<Combination> combVec = combinationVector();    
    for (auto comb=combVec.begin(); comb!=combVec.end(); comb++) {
        name = nameBase + "_residual_vs_yrel_" + comb->String();
        MakeProfileX(name, "profile");
    }
    return;
}

void ResPlots::PrintDNLPlots(string filename) {
    // Print TH2Fs
    TCanvas* c = new TCanvas();
    c->Print((filename + "[").c_str());
    TH2F* hist; // temp var
    // Print all residual combinations plot
    hist = (TH2F*)pm->Get(nameBase + "_residual_vs_yrel");
    hist->Draw("colz");
    c->Print(filename.c_str());
    c->Clear();
    // Print per combination plot
    vector<Combination> comboVec = combinationVector();
    for (auto combo=comboVec.begin(); combo!=comboVec.end(); combo++) {
        hist = (TH2F*)pm->Get(nameBase + "_residual_vs_yrel_" + combo->String());  
        if (hist->GetEntries() != 0) {
            hist->Draw("Colz");
            c->Print(filename.c_str());
            c->Clear();
        }
    }
    c->Print((filename + "]").c_str());

    // Print profiles
    TGraphErrors* graph;
    // Add _profile to input filename
    filename = filename.erase(filename.length()-4);
    filename += "_profile.pdf";
    c->Print((filename + "[").c_str());
    graph = (TGraphErrors*)pm->Get(nameBase + "_residual_vs_yrel_profile");
    graph->Draw("AP");
    c->Print(filename.c_str());
    c->Clear();
    for (auto combo=comboVec.begin(); combo!=comboVec.end(); combo++) {
        graph = (TGraphErrors*)pm->Get(nameBase + "_residual_vs_yrel_" + combo->String() + "_profile");
        graph->Draw("AP");
        c->Print(filename.c_str());
        c->Clear();
    }
    c->Print((filename + "]").c_str());
    delete c;
    return;

}

// Profile plots for differential non linearity
// Adds the 'ext' string to plot name as a name for the
// generated TGraph. New graph has exactly the same title has the input TH2F
// Returns 'true' for each successful fit
//  1. TH2F root object must exist
//  2. TH2F object must have enough entries according to AnalysisInfo
//  MODIFIED FROM tgc_analysis/src/FinalizeAnalysis.cpp:L1082
void ResPlots::MakeProfileX(string hName, string ext) {
    
  if(!pm->objExist(hName)) {
    cout << "Warning: histogram " << hName << " not profiled because it does not exist!\n\n";
    return;
 }

  TH2F *h = (TH2F*)pm->Get(hName);
  vector<Double_t> gX, gXError, gY, gYError;

  for(Int_t binX=1; binX<=h->GetNbinsX(); binX++){
    Int_t N=0; // Sum of entries in X bin
    Double_t Y=0, Y2=0; // Mean and std dev in X bin

    for(Int_t binY=1; binY<=h->GetNbinsY(); binY++){
      Int_t n = h->GetBinContent(binX, binY);
      Double_t y = h->GetYaxis()->GetBinCenter(binY);
      Y+=n*y;
      Y2+=n*y*y;
      N+=n;
    }

    if(N>3){ // Cannot calc std dev if less than 3 entries in X bin
      gX.push_back(h->GetXaxis()->GetBinCenter(binX));
      gXError.push_back(h->GetXaxis()->GetBinWidth(binX));
      gY.push_back(Y/N);

      Double_t sigma=sqrt(Y2/N-Y*Y/(N*N));
      sigma/=sqrt(N);
      gYError.push_back(sigma);
    } 
  }

  string title(h->GetTitle());
  title += ";"+string(h->GetXaxis()->GetTitle());
  title += ";Mean "+string(h->GetYaxis()->GetTitle());
  
  pm->Add(hName+"_"+ext, title, gX, gY, gXError, gYError, myTGraphErrors);

  // Do sine fit if enough stats in TGraph
  /*if(gX.size()<analysisInfo->minStatProfile)
    return false;

  TGraphErrors *gr = (TGraphErrors*) pm->Get(hName+"_"+ext);
  string fName = "fSine_"+hName;
  TF1 *fSine = new TF1(fName.c_str(),
		       "[0]*TMath::Sin(2*TMath::Pi()*x)+[1]", -0.5, 0.5);
  fSine->SetParameter(0, 0.200);
  fSine->SetParameter(1, 0.);
  fSine->SetParName(0, "A");
  fSine->SetParName(1, "ofst");

  TFitResultPtr r = gr->Fit(fName.c_str(), "MSR");
  string statusStr(gMinuit->fCstatu);

  return (statusStr.find("CONVERGED")!=string::npos);*/
  return;
}


