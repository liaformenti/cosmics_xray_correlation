#define RunAnalysis_cxx
#include "RunAnalysis.h"

using namespace std;

void RunAnalysis(TTree &trksTree, AnalysisInfo &info, PlotManager* pm, DetectorGeometry* g) {
    // TTreeReader reader(&trksTree);
    // TTreeReaderValue<Int_t> eventnumber(reader, "eventnumber");
    // TTreeReaderValue< map<UShort_t, Double_t> > trackX(reader, "trackX");
    // Declaration 
    Int_t nEntries;
    Int_t eventnumber;

    map<UShort_t, Double_t> trackX;
    map<UShort_t, Double_t>* trackXPtr;
    trackXPtr = &trackX;
    
    // Temporary uncertainty map in x
    map<UShort_t, Double_t> uncertX;
    
    map<UShort_t, Double_t> trackYGaussian;
    map<UShort_t, Double_t>* trackYGaussianPtr;
    trackYGaussianPtr = &trackYGaussian;

    // Uncertainty on y position (sigma of strip cluster gaussian fit)
    map<UShort_t, Double_t> sigma;
    map<UShort_t, Double_t>* sigmaPtr;
    sigmaPtr = &sigma;

    // Vector to store calculated residuals
    vector<Residual> residuals;

    //Initialization
    trksTree.SetBranchAddress("eventnumber", &eventnumber);
    trksTree.SetBranchAddress("trackX", &trackXPtr);
    trksTree.SetBranchAddress("trackYGaussian", &trackYGaussianPtr);
    trksTree.SetBranchAddress("sigma", &sigmaPtr);

    nEntries = trksTree.GetEntries();

    UShort_t lc = 0; UShort_t ld = 0;

    initializeUncertaintyHistograms(pm);
    // Replace i<x nEntries eventually
    // 3 events ensures you get one that passes cut 
    // with testCA.root with L3 and L4 fixed
    for (Int_t i=0; i<3; i++) {
        trksTree.GetEntry(i);
        // Uncertainty in x is width of wire group / sqrt(12)
        // Assumes uniform position distribution of hit across group
        // Some edge wires groups have less wires - later correction
        for (auto itX=trackX.begin(); itX!=trackX.end(); itX++)
            uncertX[itX->first] = 1.8*20/sqrt(12.0); 
        /*for (auto itU=uncertX.begin(); itU!=uncertX.end(); itU++)
            cout << itU->first << ' ' << itU->second << trackX[itU->first] << '\n';
        cout << "***********sigma************\n";
        for (auto itSig = sigma.begin(); itSig != sigma.end(); itSig++)
            cout << itSig->first << ' ' << itSig->second << '\n';
        cout << "***********sigma************\n";*/
        // for each permutation of two layers
        // la < lb and treated first always
        // for (Int_t la=1; la<=4; la++) {
        for (UShort_t la=3; la<=3; la++) {
            // for (Int_t lb=(la+1); lb<=4; lb++) {
            for (UShort_t lb=(la+1); lb<=4; lb++) {
                if (MissingHitsOnFixedLayers(la, lb, 
                   trackX, trackYGaussian))
                   continue;
                getOtherLayers(la, lb, &lc, &ld);
                /*cout << "RunAnalysis\n";
                cout << "  x hits " << trackX[la] << ' ' << trackX[lb] << '\n';
                cout << "  y hits " << trackYGaussian[la] << ' ' << trackYGaussian[lb] << '\n';
                cout << "  z pos " << g->GetZPosition(la) << ' ' << g->GetZPosition(lb) << '\n';*/
                Tracking myTrack(g, pm, trackX, uncertX, trackYGaussian, sigma, la, lb);


                myTrack.Fit();
                // Check if hit exists on unfixed layers
                // If so evaluate.
                Residual res;
                if (myTrack.hitsY.find(lc) != myTrack.hitsY.end()) {
                    myTrack.EvaluateAt(lc);
                    res = Residual(myTrack, lc);
                    residuals.push_back(res);

                }
                if (myTrack.hitsY.find(ld) != myTrack.hitsY.end()) {
                    myTrack.EvaluateAt(ld);
                    res = Residual(myTrack, ld);
                    residuals.push_back(res);
                }

                // myTrack.PlotFit("fits_event_" + to_string(eventnumber) + "_fixed_layers_" + to_string(la) + "_" + to_string(lb) + ".pdf");
                /*for (Int_t i=1; i<=4; i++){
                    cout << "Layer " << i << '\n';
                    cout << "X " << myTrack.trackX[i] << ' ' << myTrack.tUncertsX[i] << '\n';
                    cout << "Y " << myTrack.trackY[i] << ' ' << myTrack.tUncertsY[i] << '\n';
                }*/
            
                // cout << "Back in RunAnalysis: " << myTrackMapX[3] << '\n';
            }
        // cout << '\n'; 
        } //end for each permutation of two layers
        cout << "Iteration " << i << " of " <<  nEntries << '\n';
    } // end event loop
    StatsStudy statsStudy(&residuals, g, pm); 
    statsStudy.InitializeSquareBinHistograms(40); // mm
    statsStudy.FillSquareBinHistograms();
    statsStudy.PrintSquareBinHistograms("residuals_square_bins_width_" + to_string(statsStudy.binWidth) + "mm.pdf");
    /*// Printing residuals vector
    cout << "After loop\n";
    for (auto itr=residuals.begin(); itr!=residuals.end(); itr++) {
        cout << itr->res << '\n';
    }*/
    /* // Checking out plot manager
    pm->Add("first_plot", "title", 100, -0.5, 15.5, myTH1F);
    TRandom3 rndgen;
    for (Int_t i=0; i<400; i++) {
       pm->Fill("first_plot", rndgen.Poisson(3.6));
    }
    // pm->Fill("first_plot", 3.0);
    TH1F* myPlot = pm->GetTH1F("first_plot");
    TCanvas* c = new TCanvas();
    myPlot->Draw();
    c->Print("plot.pdf");
    delete c;*/
    return;
}

Bool_t MissingHitsOnFixedLayers(UShort_t fixed1, UShort_t fixed2, map<UShort_t, Double_t> &xTrack, map<UShort_t, Double_t> &yTrack) {
    // If one or more of the fixed layers is missing a hit in x or y,
    // return true, else return false
    Bool_t missingHit =  ( ! ( (xTrack.find(fixed1) != xTrack.end()) && 
                               (xTrack.find(fixed2) != xTrack.end()) && 
                               (yTrack.find(fixed1) != yTrack.end()) && 
                               (yTrack.find(fixed2) != yTrack.end()) ) );
    return missingHit;
}

void initializeUncertaintyHistograms(PlotManager* pm) {
    vector<Combination> combVec = combinationVector();
    string headerX = "uncertainty_x_evaluations_";
    string headerY = "uncertainty_y_evaluations_";
    for (auto v=combVec.begin(); v!=combVec.end(); v++) {
        // Just try the bin number and limits for now
        pm->Add(headerX + v->String(), headerX + v->String(),
                50, 0, 50, myTH1F);
        pm->Add(headerY + v->String(), headerY + v->String(),
                20, 0, 20, myTH1F);
    }
}
               /* for (auto itx = trackX.begin(); itx != trackX.end(); itx++) {
                     cout << itx->first << (trackX.find(itx->first) != trackX.end())<< '\n';
                     }
                }
                for (auto ity = trackYGaussian.begin(); ity != trackYGaussian.end(); ity++) {
                    cout << ity->first << (trackYGaussian.find(ity->first) != trackYGaussian.end()) << '\n';
                }*/


    // TTreeReader code:
    // Int_t count = 0;
    // while (reader.Next()) {
    //    if (count == 5) break;
    //    count++;

    /*TTreeReader trksReader(&trksTree);
    TTreeReaderValue<Int_t> eventnumber(trksReader, "eventnumber");
    TTreeReaderArray < map<UShort_t, Double_t> > trackX(trksReader, "trackX");
    Int_t i = 0;
    Double_t val = 0;
    while (trksReader.Next()) {
        if (i > 9) {
            break;
        }
        cout << *eventnumber << endl;
        val = trackX[1]
        // cout << trackX[1] << endl;
        i++;
    }*/

