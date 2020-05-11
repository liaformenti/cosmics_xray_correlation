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
    for (Int_t i=0; i<2; i++) {
        cout << z[i] << ' ' << y[i] << ' ' << ey[i] << '\n';
    }

    // Make graph
    // TGraph* graphX = new TGraph(2, x, z);
    // TGraph graphX = TGraph(2, x, z);
    // graphX = TGraphErrors(2, x, z, ex, ez);
    graphX = TGraphErrors(2, z, x, ez, ex);
    graphX.Fit("1 ++ x");
    fitX = graphX.GetFunction("1 ++ x");
    // graphX.Fit("pol1");
    // fitX = graphX.GetFunction("pol1");
    // graphX.Fit("[0] + [1]*x");
    // fitX = graphX.GetFunction("[0] + [1]*x");

    ////  TGraph graphY = TGraph(2, y, z);
    // graphY = TGraphErrors(2, y, z, ey, ez);
    graphY = TGraphErrors(2, z, y, ez, ey);
    graphY.Fit("1 ++ x");
    fitY = graphY.GetFunction("1 ++ x");
    // graphY.Fit("pol1");
    // fitY = graphY.GetFunction("pol1");
    // graphY.Fit("[0] + [1]*y");
    // fitY = graphY.GetFunction("[0] + [1]*y");

    // For plotting - make this part of Tracking, public
    /*auto c = new TCanvas();
    string name = "fitx_layer_" + to_string(la) + "_fixed_layer_" +to_string(lb) + "_fixed.pdf";
    string title = "fitx_layer_" + to_string(la) + "_fixed_layer_" +to_string(lb) + "_fixed;" + "x [mm];" + "z [mm]";
    graphX.SetMarkerStyle(kCircle);
    graphX.SetFillColor(0);
    graphX.SetTitle(title.c_str());
    graphX.Draw();
    fitX->Draw("Same");
    c->Print(name.c_str());
    delete c;*/

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
    TCanvas* can = new TCanvas();
    string title = "fitx_layer_" + to_string(la) + "_fixed_layer_" +to_string(lb) + "_fixed;" + "x [mm];" + "z [mm]";
    graphX.SetMarkerStyle(kCircle);
    graphX.SetFillColor(0);
    graphX.SetTitle(title.c_str());
    graphX.Draw();
    fitX->Draw("Same");
    can->Print(outName.c_str());
    delete can;
    return;
}
