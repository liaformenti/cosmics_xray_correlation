// Functions to build tracks from hits on 2 layers
#define Tracking_cxx
#include "Tracking.h"

using namespace std;

// Make tracks per event

Tracking::Tracking(DetectorGeometry* _g, 
                   map<UShort_t, Double_t> hitsMapX,
                   map<UShort_t, Double_t> hitsMapY)
                   : g(_g){
    cout << "called\n";
    // Tracking
    // Declaration
    hitsX = hitsMapX;
    // trackX = tracksMapX;
    hitsY = hitsMapY;
    Int_t la;
    Int_t lb;
    Double_t* fixedHitsX;
    Double_t* fixedHitsY;
    Double_t* fixedHitsZ = new Double_t[2];
    for (la=1; la<=4; la++) {
        for (lb=(la+1); lb<=4; lb++) {
            if (MissingHitsOnFixedLayers(la, lb, hitsX, hitsY))
                continue;
            cout << la << ' ' << lb << '\n';
            fixedHitsX = GetHitPoints(la, lb, hitsX);
            fixedHitsY = GetHitPoints(la, lb, hitsY); 
            fixedHitsZ[0] = g->GetZPosition(la);
            fixedHitsZ[1] = g->GetZPosition(lb);
            cout << "Tracking\n";
            cout << "  x hits " << fixedHitsX[0] << ' ' <<  fixedHitsX[1] << '\n';
            cout << "  y hits " << fixedHitsY[0] << ' ' << fixedHitsY[1] << '\n';
            
         }
    } // end perm loop
} 

Bool_t Tracking::MissingHitsOnFixedLayers(UShort_t fixed1, UShort_t fixed2, map<UShort_t, Double_t> &xTrack, map<UShort_t, Double_t> &yTrack) {
    // If one or more fixed layers are missing a hit in x or y,
    // return true, else return false
    Bool_t missingHit =  
           ( ! ( (xTrack.find(fixed1) != xTrack.end()) && 
                 (xTrack.find(fixed2) != xTrack.end()) && 
                 (yTrack.find(fixed1) != yTrack.end()) && 
                 (yTrack.find(fixed2) != yTrack.end()) ) );
    return missingHit;
}

Double_t* Tracking::GetHitPoints(Int_t la, Int_t lb, map<UShort_t, Double_t> hits) {

    Double_t* vals = new Double_t[2];
    vals[0] = hits[la];
    vals[1] = hits[lb]; 
    cout << vals[0] << ' ' << vals[1] << '\n';
    return vals;
}
/*void Track::Fit() { // Make this a part of Track class
    // Put data into arrays to be used with TGraph
    Double_t* z = new Double_t[2];
    z[0] = g->GetZPosition(la);
    z[1] = g->GetZPosition(lb);
    cout << z[0] << ' ' << z[1] << '\n';
    Double_t* x = MapToArray(trackX);
    Double_t* y = MapToArray(trackY);

    // Make graph
    // TGraph* graphX = new TGraph(2, x, z);
    TGraph graphX = TGraph(2, x, z);
    graphX.Fit("1 ++ x");
    fitX = graphX.GetFunction("1 ++ x");

    TGraph graphY = TGraph(2, y, z);
    graphY.Fit("1 ++ x");
    fitY = graphY.GetFunction("1 ++ x");

    // For plotting - make this part of Tracking, public
    auto c = new TCanvas();
    string name = "fitx_layer_" + to_string(la) + "_fixed_layer_" +to_string(lb) + "_fixed.pdf";
    string title = "fitx_layer_" + to_string(la) + "_fixed_layer_" +to_string(lb) + "_fixed;" + "x [mm];" + "z [mm]";
    graphX.SetMarkerStyle(kCircle);
    graphX.SetFillColor(0);
    graphX.SetTitle(title.c_str());
    graphX.Draw();
    fitX->Draw("Same");
    c->Print(name.c_str());
    delete c;

    delete [] z;
    delete [] x;
    delete [] y;
}*/

// public member of Tracking
/*Double_t* Tracking::MapToArray(map <UShort_t, Double_t>* theMap) {
    Double_t* vals = new Double_t[2];
    Int_t i = 0;
    for (auto const& it : *theMap) {
        vals[i] = it.second;
        i++;
    }
    return vals;
}

// Generate plots of track fits
void WriteOut(Int_t eventNumber) {
       
    return;
}*/
