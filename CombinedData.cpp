#define CombinedData_cxx
#include "CombinedData.h"

using namespace std;

// Region of interest is rectange of x (y) width of wx (wy).
// Use flag outOfRange to indicate if bin goes outside module limits
CombinedData::CombinedData(Int_t xWidth, Int_t yWidth, Int_t _ptIndex, 
                           Double_t _x, Double_t _y, UShort_t _lc, 
                           UShort_t _ld, Double_t _offC, Double_t _offD,
                           vector<Residual>* _resData,
                           DetectorGeometry* _g, PlotManager* _pm) :
    ptIndex(_ptIndex), x(_x), y(_y), lc(_lc), ld(_ld), offC(_offC), 
    offD(_offD), resData(_resData), g(_g), pm(_pm) {

    UShort_t la, lb;
    // lc and ld are layers of interest, la and lb are fixed layers
    // Note that lc < ld always
    getOtherLayers(lc, ld, &la, &lb);
    // Note that offC, smaller-value layer, is first.
    offDiff = offC - offD;
    // Fill ROI member
    // X lims is first entry, y lims is second.
    DefineRectangularROI(xWidth, yWidth);
    
    // Determine which residuals are in ROI 
    for (auto r=resData->begin(); r!=resData->end(); r++) {
        // Could replace this by creating desired combo vs res combo
        // If not on right layer
        if ((lc != r->l) && (ld != r->l)) continue;
        // If fixed layers are not right,
        if ((la != r->la) || (lb != r->lb)) continue;
        // If residual is in ROI
        if ( (ROI[0].first < r->x) && (r->x < ROI[0].second) &&
             (ROI[1].first < r->y) && (r->y < ROI[1].second)) {
           if (r->l == lc) {
               residualsInROIC.push_back(r->res);
               // cout << r->res << ' ' << r->l << ' ' << r->x << ' ' << r->y << ' ' << r->la << ' ' << r->lb << '\n';
           }
           else if (r->l == ld) {
               residualsInROID.push_back(r->res);
               // cout << r->res << ' ' << r->l << ' ' << r->x << ' ' << r->y << ' ' << r->la << ' ' << r->lb << '\n';
           }
           else {
               cout << "Warning: Logic error in CombinedData width";
               cout << "-defined rectangular ROI constructor.\n\n";
           }
        }
    }
    string name, title;
    // For lc
    WidthSpecifiedPlotNameAndTitle(name, title, lc, la, lb, xWidth, yWidth);
    // fitResultC = FitGaussian(name, title, &histC, &fitC, residualsInROIC, 200, -10, 10);
    fitResultC = FitGaussian(name, title, residualsInROIC, 200, -10, 10);
    // For ld
    WidthSpecifiedPlotNameAndTitle(name, title, ld, la, lb, xWidth, yWidth);
    // fitResultD = FitGaussian(name, title, &histD, &fitD, residualsInROID, 200, -10, 10); 
    fitResultD = FitGaussian(name, title, residualsInROID, 200, -10, 10); 

    // AND FINALLY, THE RESULT!
    meanDiff = fitResultC.mean - fitResultD.mean;
    // Error propagation . . . covariance?
    meanDiffErr = sqrt(pow(fitResultC.sigma,2) + pow(fitResultD.sigma, 2));
    return; 
} 

// This function fills lims, declared externally
void CombinedData::DefineRectangularROI(Int_t wx, Int_t wy) {
    // Define xlims
    ROI[0].first = x - wx/2.0;
    ROI[0].second = x + wx/2.0;
    // Define ylims
    ROI[1].first = y - wy/2.0;
    ROI[1].second = y + wy/2.0;
    return;
}

/*FitResult CombinedData::FitGaussian(string name, string title,
    TH1I* hist, TF1* fit, vector<Double_t>& filling, Int_t nBins, 
    Float_t lowLim, Float_t upLim) {*/
FitResult CombinedData::FitGaussian(string name, string title,
    vector<Double_t>& filling, Int_t nBins, 
    Float_t lowLim, Float_t upLim) {

    Int_t status; // Whether fit was successful (0) or not (1)
    FitResult result;
    // Book
    pm->Add(name, title, nBins, lowLim, upLim, myTH1I);
    // Fill
    for (auto f=filling.begin(); f!=filling.end(); f++) {
        pm->Fill(name, *f);
    }
    // Get TH1I
    // TH1I* localHist = (TH1I*)pm->GetTH1I(name);
    // *hist = *localHist;
    TH1I* hist = (TH1I*)pm->GetTH1I(name);
    status = hist->Fit("gaus", "SQ");
    // If it worked
    if (status==0) {
        // TF1* localFit = (TF1*)hist->GetFunction("gaus");
        // *fit = *localFit;
        TF1* fit = (TF1*)hist->GetFunction("gaus");
        result.mean = fit->GetParameter(1);
        result.meanErr = fit->GetParError(1);
        result.sigma = fit->GetParameter(2);
        result.sigErr = fit->GetParError(2);
        result.success = true;
    }
    else { // fit failed
        cout << "Warning: Gaussian fit of residuals in ROI around ";
        cout << "x-ray point failed (CombinedData::FitGaussian).\n\n";
        result.mean = 0;
        result.meanErr = 0;
        result.sigma = 0;
        result.sigErr = 0;
        result.success = false;
    }
    return result;
}

void CombinedData::WidthSpecifiedPlotNameAndTitle(string &name,
    string &title, UShort_t layer, UShort_t fixed1, UShort_t fixed2,
    Int_t xWidth, Int_t yWidth) {
    Combination combo(layer, fixed1, fixed2);
    /*name = "residuals_around_x_" + Tools::CStr(x,2) + "_y_"; 
    name += Tools::CStr(y,2);
    name += "_width_in_x_" + to_string(xWidth) + "_width_in_y_";
    name += to_string(yWidth) + "_" + combo.String();*/
    name = "residuals_around_xray_pt_" + to_string(ptIndex);
    name += "_width_in_x_" + to_string(xWidth) + "_width_in_y_";
    name += to_string(yWidth) + "_" + combo.String();
    cout << name << "\n\n";
    title = "Layer: " + to_string(combo.layer) + ", Fixed Layers: ";
    title += to_string(combo.fixed1) + to_string(combo.fixed2);
    title += ", x#in["+Tools::CStr(ROI[0].first,2)+",";
    title += Tools::CStr(ROI[0].second,2) + "], mm y#in[";
    title += Tools::CStr(ROI[1].first,2) + ","; 
    title += Tools::CStr(ROI[1].second,2) + "] mm;Residuals [mm];";
    title += "Tracks";
    return;
}

void CombinedData::PrintClassDataToFile(ofstream& f) {
    f << lc << "," << ld << "," << offC << "," << offD << ",";
    f << offDiff << ","; 
    if (fitResultC.success == true) {
        f << fitResultC.mean << "," << fitResultC.meanErr <<  ",";
        f << fitResultC.sigma << "," << fitResultC.sigErr << ",";
    }
    else { // If fit was not successful, write "NA"
        f << "NA" << "," << "NA" <<  "," << "NA" << "," << "NA" << ",";
    }
    if (fitResultD.success == true) {
        f << fitResultD.mean << "," << fitResultD.meanErr <<  ",";
        f << fitResultD.sigma << "," << fitResultD.sigErr << ",";
    }
    else { // If fit was not successful, write "NA"
        f << "NA" << "," << "NA" <<  "," << "NA" << "," << "NA" << ",";
    }
    // If both fits were successful print difference
    if ((fitResultC.success == true) && (fitResultD.success == true)) {
        f << meanDiff << "," << meanDiffErr << '\n';
    }
    else { // If not print NA
        f << "NA,NA\n";
    }
    return;
}
// Didn't quite get this to work bc appending to pdf if atomic
/*void CombinedData::PrintResHists(string filename) {
    TCanvas* c = new TCanvas();
    c->Print((filename + "[").c_str());
    c->Divide(1, 2);
    c->cd(1);
    histC.Draw();
    fitC.Draw("Same");
    c->cd(2);
    histD.Draw();
    fitD.Draw("Same");
    c->Print(filename.c_str());
    c->Print((filename + "]").c_str());
    delete c;
    return;
}*/
