#define RunAnalysis_cxx
#include "RunAnalysis.h"

using namespace std;

Bool_t MissingHitsOnFixedLayers(UShort_t fixed1, UShort_t fixed2, map<UShort_t, Double_t> &xTrack, map<UShort_t, Double_t> &yTrack) {
    // If one or more of the fixed layers is missing a hit in x or y,
    // return true, else return false
    Bool_t missingHit =  ( ! ( (xTrack.find(fixed1) != xTrack.end()) && 
                               (xTrack.find(fixed2) != xTrack.end()) && 
                               (yTrack.find(fixed1) != yTrack.end()) && 
                               (yTrack.find(fixed2) != yTrack.end()) ) );
    return missingHit;
}

void RunAnalysis(TTree &trksTree, AnalysisInfo &info, DetectorGeometry* g) {
    // TTreeReader reader(&trksTree);
    // TTreeReaderValue<Int_t> eventnumber(reader, "eventnumber");
    // TTreeReaderValue< map<UShort_t, Double_t> > trackX(reader, "trackX");
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
    // 3 events ensures you get one that passes cut with testCA.root
    for (Int_t i=0; i<3; i++) {
    // Initialize tracking class here  if you want to store tracks in tracking***
        trksTree.GetEntry(i);
        // for each permutation of two layers
        // for (Int_t la=1; la<=4; la++) {
        for (Int_t la=3; la<=3; la++) {
            // for (Int_t lb=(la+1); lb<=4; lb++) {
            for (Int_t lb=(la+1); lb<=4; lb++) {
                // if (la == lb) 
                //    continue; // don't fix the same two layers
                if (MissingHitsOnFixedLayers(la, lb, trackX, trackYGaussian))
                    continue; 
                cout << "RunAnalysis\n";
                cout << "  x hits " << trackX[la] << ' ' << trackX[lb] << '\n';
                cout << "  y hits " << trackYGaussian[la] << ' ' << trackYGaussian[lb] << '\n';
                cout << "  z pos " << g->GetZPosition(la) << ' ' << g->GetZPosition(lb) << '\n';
                map<UShort_t, Double_t> myTrackMapX;
                map<UShort_t, Double_t> myTrackMapY;
                Tracking myTrack(g, trackX, &myTrackMapX, trackYGaussian, &myTrackMapY, la, lb);
                myTrack.Fit();
                // cout << "Back in RunAnalysis: " << myTrackMapX[3] << '\n';
            }
        // cout << '\n'; 
        } //end for each permutation of two layers

        cout << '\n';
    } // end event loop

    return;
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

