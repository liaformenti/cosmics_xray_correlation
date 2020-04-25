#define RunAnalysis_cxx
#include "RunAnalysis.h"

using namespace std;

void RunAnalysis(TTree &trksTree) {

    Int_t nEntries;
    Int_t eventnumber;
    map<UShort_t, Double_t> trackX;
    map<UShort_t, Double_t>* trackXptr;
    trackXptr = &trackX;
    trksTree.SetBranchAddress("eventnumber", &eventnumber);
    trksTree.SetBranchAddress("trackX", &trackXptr);
    nEntries = trksTree.GetEntries();
    nEntries += 1;
    nEntries -= 1;

    for (Int_t i=0; i<10; i++) {
        trksTree.GetEntry(i);
        cout << trackX[1] << endl;
        printf("%f\n",trackX[1]);
        for (auto& x : trackX) {
            if (x.first == 1) {
                cout << x.second << '\n';
            }
        }
        cout << '\n';
    }

    cout << "I feel included and made" << endl;
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

