// Functions to build tracks from hits on 2 layers
#define Tracking_cxx
#include "Tracking.h"

using namespace std;

// Make tracks per event

Tracking::Tracking(DetectorGeometry* _g, 
                   PlotManager* _pm,
                   map<UShort_t, Double_t> hitsMapX,
                   map<UShort_t, Double_t> hitsUncertX,
                   map<UShort_t, Double_t> hitsMapY,
                   map<UShort_t, Double_t> hitsUncertY,
                   UShort_t fixedLayer1, UShort_t fixedLayer2)
                   : g(_g), pm(_pm) {
    // Tracking
    // Initialize members
    hitsX = hitsMapX;
    hitsXUncerts = hitsUncertX;
    hitsY = hitsMapY;
    hitsYUncerts = hitsUncertY;
    // Tracks based on hits on two fixed layers 
    la = fixedLayer1;
    lb = fixedLayer2;
    //Fill X track and Y track with fixed layer data
    fitXPos[la] = hitsX.at(la); fitXPos[lb] = hitsX.at(lb);
    fitXUncerts[la] = hitsXUncerts.at(la); 
    fitXUncerts[lb] = hitsXUncerts.at(lb);
    fitYPos[la] = hitsY.at(la); fitYPos[lb] = hitsY.at(lb);
    fitYUncerts[la] = hitsYUncerts.at(la); 
    fitYUncerts[lb] = hitsYUncerts.at(lb);
} 

void Tracking::Fit() {
    // Put data into arrays to be used with TGraph
    Double_t* z = new Double_t[2];
    z[0] = g->GetZPosition(la);
    z[1] = g->GetZPosition(lb);
    // cout << "Z " << z[0] << ' ' << z[1] << '\n';
    // Set errors in z to zero so TGraph defaults errors in z to 0
    Double_t* ez = NULL;
    Double_t* x = MapToArray(&fitXPos);
    Double_t* ex = MapToArray(&fitXUncerts);
    Double_t* y = MapToArray(&fitYPos);
    Double_t* ey= MapToArray(&fitYUncerts);
    // cout << "x " << x[0] << ' ' << x[1] << '\n';
    // cout << "y " << y[0] << ' ' << y[1] << '\n';
    // cout << "x uncert " << ex[0] << ' ' << ex[1] << '\n';
    // cout << "y uncert " << ey[0] << ' ' << ey[1] << '\n';

    // Fit
    graphX = TGraphErrors(2, z, x, ez, ex);
    resultX = graphX.Fit("pol1", "SQF");
    // cout << "CovX " << resultX->GetCovarianceMatrix()[0][1] << '\n';
    fitXFcn = graphX.GetFunction("pol1");

    graphY = TGraphErrors(2, z, y, ez, ey);
    resultY = graphY.Fit("pol1", "SQF");
    // cout << "CovY " << resultY->GetCovarianceMatrix()[0][1] << '\n';
    fitYFcn = graphY.GetFunction("pol1");

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
    fitXFcn->Draw("Same");
    can.Print(outName.c_str());

    can.Clear(); // Necessary with Draw("Same")

    graphY.SetMarkerStyle(kCircle);
    graphY.SetFillColor(0);
    graphY.SetTitle((title + "_y").c_str());
    graphY.GetYaxis()->SetTitle("y [mm]");
    graphY.GetXaxis()->SetTitle("z [mm]");
    graphY.Draw();
    fitYFcn->Draw("Same");
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

    // cout << "Eval Uncertainties: " << sigXEval << ' ' << sigYEval << '\n';
    // add points to TGraph
    graphX.SetPoint(graphX.GetN(), z, xEval);
    graphX.SetPointError(graphX.GetN()-1, 0, sigXEval);

    graphY.SetPoint(graphY.GetN(), z, yEval);
    graphY.SetPointError(graphY.GetN()-1, 0, sigYEval);

    // add points to maps
    fitXPos[layer] = xEval;
    fitXUncerts[layer] = sigXEval;
    fitYPos[layer] = yEval;
    fitYUncerts[layer] = sigYEval;

    // cout << "Tracking " << fitYPos[layer] - hitsY[layer] << ' ' << layer << ' ' << fitXPos[layer] << ' ' << fitYPos[layer] << ' ' << la << ' ' << lb << '\n';

    return;
}
