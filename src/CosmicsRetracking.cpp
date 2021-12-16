#define CosmicsRetracking_cxx
#include "CosmicsRetracking.h"

using namespace std;

CosmicsRetracking::CosmicsRetracking(TTree* _trksTree, AnalysisInfo* _cInfo, InputInfo* _myInfo,
                                     PlotManager* _pm, DetectorGeometry* _g) : Retracking(_cInfo, _myInfo, _pm, _g), trksTree(_trksTree) {
    // trksTree = _trksTree;
    nEntries = trksTree->GetEntries();
}
             
void CosmicsRetracking::Retrack() {
    // Internal variable declarations 
    Int_t eventnumber;

    map<UShort_t, Double_t> trackX;
    map<UShort_t, Double_t>* trackXPtr;
    trackXPtr = &trackX;
    
    map<UShort_t, Double_t> r_mean;
    map<UShort_t, Double_t>* r_meanPtr;
    r_meanPtr = &r_mean;

    // Uncertainty on y position (uncertainty in fitted reclustering mean)
    map<UShort_t, Double_t> r_meanError;
    map<UShort_t, Double_t>* r_meanErrorPtr;
    r_meanErrorPtr = &r_meanError;

    //Initialization TTree branches
    trksTree->SetBranchAddress("eventnumber", &eventnumber);
    trksTree->SetBranchAddress("trackX", &trackXPtr);
    trksTree->SetBranchAddress("r_mean", &r_meanPtr);
    trksTree->SetBranchAddress("r_meanError", &r_meanErrorPtr);

    InitializeTrackUncertaintyHistograms();
    InitializeTrackAngleHistograms();
    InitializeResidualUncertaintyHistograms();

    // For each entry, do retracking for each set of fixed layers
    // cout << "FEW TRACKS ONLY (CosmicsRetracking)\n";
    for (Int_t i=0; i<nEntries; i++) {
        trksTree->GetEntry(i);
        // Uncertainty in x is width of wire group / sqrt(12)
        // Assumes uniform position distribution of hit across group
        // Some edge wires groups have less wires - later correction
        map<UShort_t, Double_t> uncertX;
        for (auto itX=trackX.begin(); itX!=trackX.end(); itX++)
            uncertX[itX->first] = 1.8*20/sqrt(12.0); // mm

        // Uncertainty in y is spread of mean_reclustering - mean_cosmics distribution for 3100V
        // (systematic error)
        map<UShort_t, Double_t> uncertY;
        for (auto itY=r_mean.begin(); itY!=r_mean.end(); itY++)
            uncertY[itY->first] = 0.06; // mm

        // for each permutation of two layers
        // la < lb and la is treated first always
        // la and lb are private members
        for (la=1; la<=4; la++) {
        // for (la=1; la <=1; la++) {
            for (lb=(la+1); lb<=4; lb++) {
            // for (lb=4; lb<=4; lb++) {

               if (MissingHitsOnFixedLayers(trackX, r_mean))
                   continue;

                getOtherLayers(la, lb, &lc, &ld);
                // Tracking myTrack(g, pm, trackX, uncertX, r_mean, r_meanError, la, lb);
                // With uncertY:
                Tracking myTrack(g, pm, trackX, uncertX, r_mean, uncertY, la, lb);

                myTrack.Fit();

                // y-track angle cut
                /*if (cutAngle && 
                   (abs(tan(myTrack.resultY->Value(1))) > angleCut)) {
                    continue; 
                }*/

                // Check if hit exists on unfixed layers
                // If so evaluate and calculate residual
                Residual res;
                if (myTrack.hitsY.find(lc) != myTrack.hitsY.end()) {
                    myTrack.EvaluateAt(lc);
                    pm->Fill("uncertainty_y_evaluations_" + Combination(lc, la, lb).String(), 
                             myTrack.fitYUncerts.at(lc));
                    pm->Fill("track_y_angle_" + Combination(lc, la, lb).String(), 
                             tan(myTrack.resultY->Value(1)));
                    res = Residual(myTrack, lc);
                    pm->Fill("residual_uncertainty_" + Combination(lc, la, lb).String(), 
                             res.resErr);
                    residuals.push_back(res);
                }
                if (myTrack.hitsY.find(ld) != myTrack.hitsY.end()) {
                    myTrack.EvaluateAt(ld);
                    pm->Fill("uncertainty_y_evaluations_" + Combination(ld, la, lb).String(), 
                             myTrack.fitYUncerts.at(ld));
                    pm->Fill("track_y_angle_" + Combination(ld, la, lb).String(), 
                             tan(myTrack.resultY->Value(1)));
                    res = Residual(myTrack, ld);
                    pm->Fill("residual_uncertainty_" + Combination(ld, la, lb).String(), 
                             res.resErr);
                    residuals.push_back(res);
                }
                // Plot linear fit
                /*if (i==0) {
                    myTrack.PlotFit(myInfo->outpath + "fits_event_" + to_string(eventnumber) + "_fixed_layers_" + to_string(la) + "_" + to_string(lb) + ".pdf");
                }*/
            } // end ld loop
        } // end lc loop
        // Count iterations
        if (i%10000==0) {
            cout << "Track " << i << " of " <<  nEntries << '\n';
        }
    } // end event loop

    return;
}

// CHANGED FIXED1/2 TO PRIVATE LA LB MEMBERS
/*Bool_t CosmicsRetracking::MissingHitsOnFixedLayers(map<UShort_t, Double_t> &xTrack, 
                                                   map<UShort_t, Double_t> &yTrack) {
    // If one or more of the fixed layers is missing a hit in x or y,
    // return true, else return false
    Bool_t missingHit =  ( ! ( (xTrack.find(la) != xTrack.end()) && 
                               (xTrack.find(lb) != xTrack.end()) && 
                               (yTrack.find(la) != yTrack.end()) && 
                               (yTrack.find(lb) != yTrack.end()) ) );
    return missingHit;
}*/

void CosmicsRetracking::InitializeTrackUncertaintyHistograms() {
    vector<Combination> combVec = combinationVector();
    string headerY = "uncertainty_y_evaluations_";
    for (auto v=combVec.begin(); v!=combVec.end(); v++) {
        // Just try the bin number and limits for now
        pm->Add(headerY + v->String(), "Layer: " + to_string(v->layer) + ", Fixed layers: " + 
                to_string(v->fixed1) + to_string(v->fixed2) + 
                " y-track fit uncertainty;Uncertainty [mm];Tracks;", 30, 0, 0.15, myTH1F);
    }
    return;
}

void CosmicsRetracking::PrintTrackUncertaintyHistograms() {
    TCanvas *c = new TCanvas();
    string outName = myInfo->outpath + myInfo->tag + myInfo->quadname;
    outName += "_y_evaluation_uncertainties.pdf";
    c->Print((outName + "[").c_str());
    vector<Combination> combVec = combinationVector();
    for (auto comb=combVec.begin(); comb!=combVec.end(); comb++) {
        TH1F* uyhist = (TH1F*)pm->GetTH1F("uncertainty_y_evaluations_" + comb->String());
        uyhist->Draw();
        c->Print(outName.c_str());
    }
    c->Print((outName + "]").c_str());
    delete c;
    return;
}

void CosmicsRetracking::InitializeTrackAngleHistograms() {
    vector<Combination> combVec = combinationVector(); 
    string name, title;
    string headerY = "track_y_angle_";
    for (auto v=combVec.begin(); v!=combVec.end(); v++) { 
        name = headerY + v->String();
        title = "Layer: " + to_string(v->layer) + ", Fixed layers: " + to_string(v->fixed1); 
        title+= to_string(v->fixed2) + " - y-track angles;Angle [rads];Tracks";
        pm->Add(name, title, 120, -3.14/2, 3.14/2, myTH1I);
    }
    return;
}

void CosmicsRetracking::PrintTrackAngleHistograms() {
    TCanvas *c = new TCanvas();
    string outName = myInfo->outpath + myInfo->tag + myInfo->quadname + "_track_y_angle_hists.pdf";
    c->Print((outName + "[").c_str());
    vector<Combination> combVec = combinationVector();
    for (auto v=combVec.begin(); v!=combVec.end(); v++) {
        TH1I* anghist = (TH1I*)pm->GetTH1I("track_y_angle_" + v->String());             
        anghist->Draw();
        c->Print(outName.c_str());
    }
    c->Print((outName + "]").c_str());
    delete c;
    return;
}

void CosmicsRetracking::InitializeResidualUncertaintyHistograms() {
    vector<Combination> combVec = combinationVector(); 
    string name, title;
    string headerY = "residual_uncertainty_";
    for (auto v=combVec.begin(); v!=combVec.end(); v++) { 
        name = headerY + v->String();
        title = "Layer: " + to_string(v->layer) + ", Fixed layers: " + to_string(v->fixed1); 
        title+= to_string(v->fixed2) + " - residual uncertainties;Uncertainty [mm];Tracks";
        pm->Add(name, title, 30, 0, 0.15, myTH1I);

    }
    return;
}

void CosmicsRetracking::PrintResidualUncertaintyHistograms() {
    TCanvas *c = new TCanvas();
    string outName = myInfo->outpath + myInfo->tag + myInfo->quadname;
    outName += "_residual_uncertainty_hists.pdf";
    c->Print((outName + "[").c_str());
    vector<Combination> combVec = combinationVector();
    for (auto v=combVec.begin(); v!=combVec.end(); v++) {
        TH1I* anghist = (TH1I*)pm->GetTH1I("residual_uncertainty_" + v->String());             
        anghist->Draw();
        c->Print(outName.c_str());
    }
    c->Print((outName + "]").c_str());
    delete c;
    return;
}


