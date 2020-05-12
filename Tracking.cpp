// Functions to build tracks from hits on 2 layers
#define Tracking_cxx
#include "Tracking.h"

using namespace std;

// Make tracks per event

Tracking::Tracking(DetectorGeometry* _g, 
                   map<UShort_t, Double_t> hitsMapX,
                   map<UShort_t, Double_t> hitsUncertX,
                   map<UShort_t, Double_t> tracksMapX, 
                   map<UShort_t, Double_t> trackUncertsX, 
                   map<UShort_t, Double_t> hitsMapY,
                   map<UShort_t, Double_t> hitsUncertY,
                   map<UShort_t, Double_t> tracksMapY, 
                   map<UShort_t, Double_t> trackUncertsY, 
                   UShort_t fixedLayer1, UShort_t fixedLayer2)
                   : g(_g){
    // Tracking
    // Declaration
    hitsX = hitsMapX;
    hUncertsX = hitsUncertX;
    trackX = tracksMapX;
    tUncertsX = trackUncertsX;
    hitsY = hitsMapY;
    hUncertsY = hitsUncertY;
    trackY = tracksMapY;
    tUncertsY = trackUncertsY;
    // Tracks based on hits on two fixed layers 
    la = fixedLayer1;
    lb = fixedLayer2;
    //Fill X track and Y track with fixed layer data
    trackX[la] = hitsX[la]; trackX[lb] = hitsX[lb];
    tUncertsX[la] = hUncertsX[la]; tUncertsX[lb] = hUncertsX[lb];
    trackY[la] = hitsY[la]; trackY[lb] = hitsY[lb];
    tUncertsY[la] = hUncertsY[la]; tUncertsY[lb] = hUncertsY[lb];
    
    // Track
    TGraphErrors graphX;
    TF1* fitX;
    TGraphErrors graphY;
    TF1* fitY;
} 

void Tracking::Fit() {
    // Put data into arrays to be used with TGraph
    Double_t* z = new Double_t[2];
    z[0] = g->GetZPosition(la);
    z[1] = g->GetZPosition(lb);
    // Set errors in z to zero so TGraph defaults errors in z to 0
    Double_t* ez = NULL;
    Double_t* x = MapToArray(&trackX);
    Double_t* ex = MapToArray(&tUncertsX);
    Double_t* y = MapToArray(&trackY);
    Double_t* ey= MapToArray(&tUncertsY);

    // Fit
    graphX = TGraphErrors(2, z, x, ez, ex);
    graphX.Fit("1 ++ x");
    fitX = graphX.GetFunction("1 ++ x");

    graphY = TGraphErrors(2, z, y, ez, ey);
    graphY.Fit("1 ++ x");
    fitY = graphY.GetFunction("1 ++ x");

    delete [] z;
    delete [] x;
    delete [] ex;
    delete [] y;
    delete [] ey;
}

Double_t* Tracking::MapToArray(map <UShort_t, Double_t>* theMap) {
    Double_t* vals = new Double_t[2];
    Int_t i = 0;
    for (auto const& it : *theMap) {
        vals[i] = it.second;
        i++;
    }
    return vals;
}

// Generate plots of track fits
void Tracking::PlotFit(string outName) {
    TCanvas can("can");
    string title = "fit_layer_" + to_string(la) + "_fixed_layer_" +to_string(lb) + "_fixed";

    can.Print((outName + "[").c_str());

    graphX.SetMarkerStyle(kCircle);
    graphX.SetFillColor(0);
    graphX.SetTitle((title + "_x").c_str());
    graphX.GetYaxis()->SetTitle("x [mm]");
    graphX.GetXaxis()->SetTitle("z [mm]");
    graphX.Draw();
    fitX->Draw("Same");
    can.Print(outName.c_str());

    can.Clear(); // Necessary with Draw("Same")

    graphY.SetMarkerStyle(kCircle);
    graphY.SetFillColor(0);
    graphY.SetTitle((title + "_y").c_str());
    graphY.GetYaxis()->SetTitle("y [mm]");
    graphY.GetXaxis()->SetTitle("z [mm]");
    graphY.Draw();
    fitY->Draw("Same");
    can.Print(outName.c_str());

    can.Print((outName + "]").c_str());

    return;
}

void Tracking::EvaluateAt(Double_t z) {
    cout << z << '\n';
    return;
}
