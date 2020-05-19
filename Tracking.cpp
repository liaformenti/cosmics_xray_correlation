// Functions to build tracks from hits on 2 layers
#define Tracking_cxx
#include "Tracking.h"

using namespace std;

// Make tracks per event

Tracking::Tracking(DetectorGeometry* _g, 
                   map<UShort_t, Double_t> hitsMapX,
                   map<UShort_t, Double_t> hitsUncertX,
                   // map<UShort_t, Double_t> tracksMapX, 
                   // map<UShort_t, Double_t> trackUncertsX, 
                   map<UShort_t, Double_t> hitsMapY,
                   map<UShort_t, Double_t> hitsUncertY,
                   // map<UShort_t, Double_t> tracksMapY, 
                   // map<UShort_t, Double_t> trackUncertsY, 
                   UShort_t fixedLayer1, UShort_t fixedLayer2)
                   : g(_g){
    // Tracking
    // Declaration
    hitsX = hitsMapX;
    hUncertsX = hitsUncertX;
    // trackX = tracksMapX;
    // tUncertsX = trackUncertsX;
    hitsY = hitsMapY;
    hUncertsY = hitsUncertY;
    // trackY = tracksMapY;
    // tUncertsY = trackUncertsY;
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
    TFitResultPtr resultX;
    TGraphErrors graphY;
    TF1* fitY;
    TFitResultPtr resultY;
} 

void Tracking::Fit() {
    // Put data into arrays to be used with TGraph
    Double_t* z = new Double_t[2];
    z[0] = g->GetZPosition(la);
    z[1] = g->GetZPosition(lb);
    cout << "Z " << z[0] << ' ' << z[1] << '\n';
    // Set errors in z to zero so TGraph defaults errors in z to 0
    Double_t* ez = NULL;
    Double_t* x = MapToArray(&trackX);
    Double_t* ex = MapToArray(&tUncertsX);
    Double_t* y = MapToArray(&trackY);
    Double_t* ey= MapToArray(&tUncertsY);
    cout << "x " << x[0] << ' ' << x[1] << '\n';
    cout << "y " << y[0] << ' ' << y[1] << '\n';
    cout << "x uncert " << ex[0] << ' ' << ex[1] << '\n';
    cout << "y uncert " << ey[0] << ' ' << ey[1] << '\n';

    // Fit
    graphX = TGraphErrors(2, z, x, ez, ex);
    resultX = graphX.Fit("1 ++ x", "S");
    cout << "CovX " << resultX->GetCovarianceMatrix()[0][1] << '\n';
    fitX = graphX.GetFunction("1 ++ x");

    graphY = TGraphErrors(2, z, y, ez, ey);
    resultY = graphY.Fit("1 ++ x", "S");
    cout << "CovY " << resultY->GetCovarianceMatrix()[0][1] << '\n';
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

void Tracking::EvaluateAt(UShort_t layer) {
    Double_t z = g->GetZPosition(layer);
    // Evaluate x position
    Double_t bx = resultX->Value(0); 
    Double_t sigbx = resultX->ParError(0); 
    Double_t mx = resultX->Value(1);
    Double_t sigmx = resultX->ParError(1);
    TMatrixD covX = resultX->GetCovarianceMatrix();
    // Evaluate x at z
    Double_t xEval = mx*z + bx;
    Double_t sigXEval = sqrt(pow(sigmx*z,2) + pow(sigbx,2) + 2*z*covX[0][1]);
    // Evaluate y position
    Double_t by = resultY->Value(0);
    Double_t sigby = resultY->ParError(0);
    Double_t my = resultY->Value(1);
    Double_t sigmy = resultY->ParError(1);
    TMatrixD covY = resultY->GetCovarianceMatrix();
    // Evaluate y at z
    Double_t yEval = my*z + by;
    Double_t sigYEval = sqrt(pow(sigmy*z,2) + pow(sigby,2) + 2*z*covY[0][1]);

    cout << "Eval Uncertainties: " << sigXEval << ' ' << sigYEval << '\n';
    // add points to TGraph
    graphX.SetPoint(graphX.GetN(), z, xEval);
    graphX.SetPointError(graphX.GetN()-1, 0, sigXEval);

    graphY.SetPoint(graphY.GetN(), z, yEval);
    graphY.SetPointError(graphY.GetN()-1, 0, sigYEval);

    // add points to maps
    trackX[layer] = xEval;
    tUncertsX[layer] = sigXEval;
    trackY[layer] = yEval;
    tUncertsY[layer] = sigYEval;
    return;
}
