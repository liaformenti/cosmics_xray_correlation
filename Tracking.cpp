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
    /*trackX->insert( pair<UShort_t, Double_t> (la, hitsX[la]) );
    trackX->insert( pair<UShort_t, Double_t> (lb, hitsX[lb]) );
    tUncertsX->insert( pair<UShort_t, Double_t> (la, hUncertsX[la]) );
    tUncertsX->insert( pair<UShort_t, Double_t> (lb, hUncertsX[lb]) );
    trackY->insert( pair<UShort_t, Double_t> (la, hitsY[la]) );
    trackY->insert( pair<UShort_t, Double_t> (lb, hitsY[lb]) );
    tUncertsY->insert( pair<UShort_t, Double_t> (la, hUncertsY[la]) );
    tUncertsY->insert( pair<UShort_t, Double_t> (lb, hUncertsY[lb]) );*/
    trackX[la] = hitsX[la]; trackX[lb] = hitsX[lb];
    tUncertsX[la] = hUncertsX[la]; tUncertsX[lb] = hUncertsX[lb];
    trackY[la] = hitsY[la]; trackY[lb] = hitsY[lb];
    tUncertsY[la] = hUncertsY[la]; tUncertsY[lb] = hUncertsY[lb];
    
    // Track
    TGraph graphX;
    TF1* fitX;
    TGraph graphY;
    TF1* fitY;
} 

void Tracking::Fit() {
    // Put data into arrays to be used with TGraph
    Double_t* z = new Double_t[2];
    z[0] = g->GetZPosition(la);
    z[1] = g->GetZPosition(lb);
    Double_t* x = MapToArray(&trackX);
    Double_t* y = MapToArray(&trackY);
    /*for (Int_t j=0; j < 2; j++)
        cout << x[j] << ' ' << y[j] << ' ' << z[j] << '\n';*/

    // Make graph
    // TGraph* graphX = new TGraph(2, x, z);
    // TGraph graphX = TGraph(2, x, z);
    graphX = TGraph(2, x, z);
    graphX.Fit("1 ++ x");
    fitX = graphX.GetFunction("1 ++ x");

    ////  TGraph graphY = TGraph(2, y, z);
    graphY = TGraph(2, y, z);
    graphY.Fit("1 ++ x");
    fitY = graphY.GetFunction("1 ++ x");

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
    delete [] y;
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
void Tracking::PlotFit(string name) {
    TCanvas* can = new TCanvas();
    string title = "fitx_layer_" + to_string(la) + "_fixed_layer_" +to_string(lb) + "_fixed;" + "x [mm];" + "z [mm]";
    graphX.SetMarkerStyle(kCircle);
    graphX.SetFillColor(0);
    graphX.SetTitle(title.c_str());
    graphX.Draw();
    fitX->Draw("Same");
    can->Print(name.c_str());
    delete can;
    return;
}
