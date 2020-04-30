#define RunAnalysis_cxx
#include "RunAnalysis.h"

using namespace std;

Bool_t MissingHitsOnFixedLayers(UShort_t fixed1, UShort_t fixed2, map<UShort_t, Double_t> &xTrack, map<UShort_t, Double_t> &yTrack) {
    // If one or more of the fixed layers is missing a hit in x or y,
    // return true, else return false
    Bool_t missingHit =  ( ! ((xTrack.find(fixed1) != xTrack.end()) && (xTrack.find(fixed2) != xTrack.end()) && (yTrack.find(fixed1) != yTrack.end()) && (yTrack.find(fixed2) != yTrack.end()) ) );
    cout << missingHit << '\n';
    return missingHit;
}

void RunAnalysis(TTree &trksTree, AnalysisInfo &info, DetectorGeometry &g) {
    Int_t nEntries;
    Int_t eventnumber;
    map<UShort_t, Double_t> trackX;
    map<UShort_t, Double_t>* trackXPtr;
    trackXPtr = &trackX;
    map<UShort_t, Double_t> trackYGaussian;
    map<UShort_t, Double_t>* trackYGaussianPtr;
    trackYGaussianPtr = &trackYGaussian;
    trksTree.SetBranchAddress("eventnumber", &eventnumber);
    trksTree.SetBranchAddress("trackX", &trackXPtr);
    trksTree.SetBranchAddress("trackYGaussian", &trackYGaussianPtr);
    nEntries = trksTree.GetEntries();
    // Prevent warning about unused nEntries while not doing full event loop
    nEntries += 1;
    nEntries -= 1;
    // Replace i<x nEntries eventually
    for (Int_t i=0; i<5; i++) {
        trksTree.GetEntry(i);

        // for each permutation of two layers
        // for (Int_t la=1; la<=4; la++) {
        for (Int_t la=3; la<4; la++) {
            // for (Int_t lb=1; lb<=4; lb++) {
            for (Int_t lb=4; lb<5; lb++) {
                if (la == lb) { continue; } // don't fix the same two layers
                //if (!((trackX.find(la) != trackX.end()) && (trackX.find(lb) != trackX.end()) && (trackYGaussian.find(la) != trackYGaussian.end()) && (trackYGaussian.find(lb) != trackYGaussian.end()))) { continue;}
                  if (MissingHitsOnFixedLayers(la, lb, trackX, trackYGaussian)) { continue; }
                for (auto itx = trackX.begin(); itx != trackX.end(); itx++) {
                     cout << itx->first << (trackX.find(itx->first) != trackX.end())<< '\n';
                     if (trackX.find(itx->first) != trackX.end()) {
                         cout << "True\n";
                     }
                }
                for (auto ity = trackYGaussian.begin(); ity != trackYGaussian.end(); ity++) {
                    cout << ity->first << (trackYGaussian.find(ity->first) != trackYGaussian.end()) << '\n';
                }
            }
        cout << endl; 
        } //end for each permutation of two layers

        cout << '\n';
    } // end event loop

    return;
}


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

