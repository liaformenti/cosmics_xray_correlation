#define RunAnalysis_cxx
#include "RunAnalysis.h"

using namespace std;

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
    for (Int_t i=0; i<1; i++) {
        trksTree.GetEntry(i);
        // for each permutation of two layers
        // for (Int_t la=1; la<=4; la++) {
        for (Int_t la=3; la<4; la++) {
            // for (Int_t lb=1; lb<=4; lb++) {
            for (Int_t lb=4; lb<5; lb++) {
                if (la == lb) { continue; } // don't fix the same two layers
                for (auto itx = trackX.begin(); itx != trackX.end(); itx++) {
                    cout << itx->first << '\n';
                }
                for (auto ity = trackYGaussian.begin(); ity != trackYGaussian.end(); ity++) {
                    cout << ity->first << '\n';
                }
 
            }
        } //end for each permutation of two layers
        cout << '\n';
    } // end even loop

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

