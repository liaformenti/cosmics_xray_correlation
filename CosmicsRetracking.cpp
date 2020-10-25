#define CosmicsRetracking_cxx
#include "CosmicsRetracking.h"

using namespace std;

CosmicsRetracking::CosmicsRetracking(TTree* _trksTree, AnalysisInfo* _cInfo, InputInfo* _myInfo,
                                     PlotManager* _pm, DetectorGeometry* _g) : trksTree(_trksTree),
                                     cInfo(_cInfo), myInfo(_myInfo), pm(_pm), g(_g) {
    nEntries = trksTree->GetEntries();
}
             
void CosmicsRetracking::Retrack() {
    // Internal variable declarations 
    Int_t eventnumber;

    map<UShort_t, Double_t> trackX;
    map<UShort_t, Double_t>* trackXPtr;
    trackXPtr = &trackX;
    
    // Uncertainty in x hits
    // Initialized in loop to only have entries on layers with hits
    map<UShort_t, Double_t> uncertX;
    
    map<UShort_t, Double_t> trackYGaussian;
    map<UShort_t, Double_t>* trackYGaussianPtr;
    trackYGaussianPtr = &trackYGaussian;

    // Uncertainty on y position (sigma of strip cluster gaussian fit)
    map<UShort_t, Double_t> sigma;
    map<UShort_t, Double_t>* sigmaPtr;
    sigmaPtr = &sigma;

    //Initialization TTree branches
    trksTree->SetBranchAddress("eventnumber", &eventnumber);
    trksTree->SetBranchAddress("trackX", &trackXPtr);
    trksTree->SetBranchAddress("trackYGaussian", &trackYGaussianPtr);
    trksTree->SetBranchAddress("sigma", &sigmaPtr);

    return;
}

// CHANGED FIXED1/2 TO PRIVATE LA LB MEMBERS
Bool_t CosmicsRetracking::MissingHitsOnFixedLayers(map<UShort_t, Double_t> &xTrack, 
                                                   map<UShort_t, Double_t> &yTrack) {
    // If one or more of the fixed layers is missing a hit in x or y,
    // return true, else return false
    Bool_t missingHit =  ( ! ( (xTrack.find(la) != xTrack.end()) && 
                               (xTrack.find(lb) != xTrack.end()) && 
                               (yTrack.find(la) != yTrack.end()) && 
                               (yTrack.find(lb) != yTrack.end()) ) );
    return missingHit;
}

void CosmicsRetracking::InitializeTrackUncertaintyHistograms() {
    vector<Combination> combVec = combinationVector();
    string headerY = "uncertainty_y_evaluations_";
    for (auto v=combVec.begin(); v!=combVec.end(); v++) {
        // Just try the bin number and limits for now
        pm->Add(headerY + v->String(), "Layer: " + to_string(v->layer) + ", Fixed layers: " + to_string(v->fixed1) + to_string(v->fixed2) + " y-track fit uncertainty;Uncertainty [mm];Tracks;", 60, 0, 20, myTH1F);
    }
    return;
}

void CosmicsRetracking::PrintTrackUncertaintyHistograms() {
    TCanvas *c = new TCanvas();
    string outName = myInfo->outpath + "y_evaluation_uncertainties.pdf";
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

// CHANGED LA LB to L1 L2 (SINCE LA LB ARE PRIVATE MEMBERS)
void CosmicsRetracking::InitializeTrackAngleHistograms() {
    // One hist for each fixed l1yer combination for x and for y
  string name, title;
  string coord[2] = {"x", "y"};
  for (UShort_t l1=1; l1<=4; l1++) {
    for (UShort_t l2=(l1+1); l2<=4; l2++) {
        for (UShort_t i=0; i<2; i++) {
         name = "track_angle_" + coord[i] + "_fixed_l1yers_"; 
         name += to_string(l1) + to_string(l2);             
         title = "Fixed l1yers: " + to_string(l1) + to_string(l2);
         title += " " + coord[i] + "-track angles;Angle [rads];Tracks";
         pm->Add(name, title, 120, -3.14/2, 3.14/2, myTH1I);
       }
     }
   }
   return;
}

// CHANGED LA LB to L1 L2 (SINCE LA LB ARE PRIVATE MEMBERS)
void CosmicsRetracking::PrintTrackAngleHistograms() {
  TCanvas *c = new TCanvas();
  string outName = myInfo->outpath + "track_angle_hists.pdf";
  c->Print((outName + "[").c_str());
  string coord[2] = {"x", "y"};

  for (UShort_t i=0; i<2; i++) {
    for (UShort_t l1=1; l1<=4; l1++) {
      for (UShort_t l2=(l1+1); l2<=4; l2++) {
        TH1I* anghist = (TH1I*)pm->GetTH1I("track_angle_" + coord[i] + "_fixed_l1yers_" + to_string(l1) + to_string(l2));             
        anghist->Draw();
        c->Print(outName.c_str());
      }
    }
  }
  c->Print((outName + "]").c_str());
  delete c;
  return;
}

