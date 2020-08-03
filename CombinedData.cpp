#define CombinedData_cxx
#include "CombinedData.h"

using namespace std;

CombinedData::CombinedData(XRayPt xPt, UShort_t layerA,
  UShort_t layerB, vector<Residual>* _resData, DetectorGeometry* _g,
  PlotManager* _pm) : resData(_resData), g(_g), pm(_pm) {
    // Copy argument data into members
    xPtIndex = xPt.num;
    x = xPt.xnom;
    y = xPt.ynom;   
   
    // Check that offsets exist for each input layer
    // If not throw exception and prevent construction
    if (xPt.offsets.find(layerA) == xPt.offsets.end()) {
        throw invalid_argument("Invalid argument: no offset exists for layerA, " + to_string(layerA) + ".\n\n");
    }
    else if (xPt.offsets.find(layerB) == xPt.offsets.end()) {
        throw invalid_argument("Invalid argument: no offset exists for layerB, " + to_string(layerB) + ".\n\n");
    }

    // Assign smaller layer to la, larger to lb (consistency)
    if (layerA < layerB) {
        la = layerA;
        lb = layerB;
    }
    else if (layerA > layerB) {
        la = layerB;
        lb = layerA;
    }
    else { // they are equal
        throw invalid_argument("Invalid argument: both layers are the same (CombinedData constructor).\n\n");
    }
    
    // Get fixed layers, lc, ld, from layers of interest, la, lb.
    getOtherLayers(la, lb, &lc, &ld);

    // Init layer spec data
    PtLayerData first, second;
    first.offset = xPt.offsets.at(la);
    first.offsetError = xPt.offsetErrors.at(la);
    second.offset = xPt.offsets.at(lb);
    second.offsetError = xPt.offsetErrors.at(lb);

    // Placeholder initialization.
    first.residualsInROI = {}, second.residualsInROI = {};
    first.histName = "", second.histName="";
    // Use fit false as a marker for before fit and failed fit
    first.success = false, second.success = false;
    // Init fit params to zero
    first.mean = 0, second.mean = 0;
    first.meanError = 0, second.meanError = 0;
    first.sigma = 0, second.sigma = 0;
    first.sigmaError = 0, second.sigmaError = 0;
    first.amplitude = 0, second.amplitude = 0;

    // Put points in map
    layerData.insert(pair<UShort_t, PtLayerData>(la, first));
    layerData.insert(pair<UShort_t, PtLayerData>(lb, second));

    // Calculate difference in xray offsets
    offDiff = layerData.at(la).offset - layerData.at(lb).offset;
    offDiffError = sqrt(pow(layerData.at(la).offsetError, 2) +
                        pow(layerData.at(lb).offsetError, 2));
    // Init residual mean difference to zero
    meanDiff = 0;
    meanDiffError = 0;

    return;
}

void CombinedData::DefineRectangularROI(Double_t xWidth, 
                                        Double_t yWidth) {
    if ((xWidth <= 0) || (yWidth <= 0)) {
        throw invalid_argument("Please give positive widths (CombinedData::DefineRectangularROI).\n\n");
    }
    xROI.first = x - xWidth/2.0;
    xROI.second = x + xWidth/2.0;
    yROI.first = y - yWidth/2.0;
    yROI.second = y + yWidth/2.0;
    return;
}

void CombinedData::FillROIsWithResiduals() {
    // If ROIs are not assigned,
    Float_t eps = 0.001;
    if ( ( (abs(xROI.first) < eps) && (abs(xROI.second) < eps) ) ||
         ( (abs(yROI.first) < eps) && (abs(yROI.second < eps)) ) ) {
        throw logic_error("Region of interest around xray point not defined (CombinedData::FillROIWithResiduals).\n\n");
    }

    for (auto r=resData->begin(); r!=resData->end(); r++) {
        // If residuals is not on right layer, skip.
        if ((r->l != la) && (r->l != lb)) continue;
        // If fixed layers are not right, skip.
        // Note confusing naming convention!
        // For Residual struct la and lb are fixed layers
        // but lc and ld and fixed layers here,
        if ((r->la != lc) || (r->lb != ld)) continue;
        // If residual is in ROI
        if ( (r->x > xROI.first) && (r->x < xROI.second) &&
             (r->y > yROI.first) && (r->y < yROI.second) ) {
           layerData.at(r->l).residualsInROI.push_back(r->res);
        }
    }
    return;
}

void CombinedData::CreateResidualHistograms() {
    string name, title;
    Combination combo;
    // For layers of interest,
    for (auto pld=layerData.begin(); pld!=layerData.end(); pld++) {
        
        // Build name and title
        // Might make this a method so I can use same naming scheme
        // for other fit types
        combo = Combination(pld->first, lc, ld);
        name = "residuals_around_xray_point_" + to_string(xPtIndex);
        name += "_width_in_x_" +Tools::CStr(xROI.second-xROI.first, 2);        name += "_width_in_y_" +Tools::CStr(yROI.second-yROI.first, 2);
        name += "_" + combo.String();
        title = "Layer: " + to_string(combo.layer) +", Fixed Layers: ";
        title += to_string(combo.fixed1) + to_string(combo.fixed2);
        title += ", x#in["+Tools::CStr(xROI.first,2) + ",";
        title += Tools::CStr(xROI.second,2) + "], mm y#in[";
        title += Tools::CStr(yROI.first, 2) + ",";
        title += Tools::CStr(yROI.second,2) + "] mm;Residuals [mm];";
        title += "Tracks";
        
        // Book TH1I
        pm->Add(name, title, 200, -10, 10, myTH1I);
        
        // Add name to PtLayerData struct
        pld->second.histName = name;

        // Print warning if residuals is empty (in case user forgot
        // to call FillROIsWithResiduals method
        if (pld->second.residualsInROI.size() == 0) {
            cout << "Warning: list of residuals in ROI around xray ";
            cout << "point, " << xPtIndex << ", on layer, ";
            cout << pld->first << ", is empty. Are you calling the ";
            cout << "FillROIsWithResiduals method ";
            cout << "(CombinedData::CreateResidualHistograms)?\n\n";
        }
        // Fill
        for (auto res=pld->second.residualsInROI.begin();
                  res!=pld->second.residualsInROI.end(); res++) {
            pm->Fill(name, *res);
        }
    }
    return;
}

void CombinedData::FitGaussian() {
    TH1I* hist;
    TF1* fit;
    Int_t status; // To hold fit return value

    for (auto pld=layerData.begin(); pld!=layerData.end(); pld++) {
        // Do gaus fit
        hist = (TH1I*)pm->GetTH1I(pld->second.histName);

        // Don't bother fitting an empty histogram. Fit success
        // flag in layer data will remain false.
        if (hist->GetEntries() == 0) continue;

        // If you want to have the fit stored in plot manager,
        // could define function, giving it a name, add it to pm, 
        // fit it, get out the custom params, and carry on
        status = hist->Fit("gaus", "Q");

        if (status == 0) { // If fit was successful,
            pld->second.success = true;
            // fit = (TF1*)pm->Get(name + "_gaus_fit");
            fit = (TF1*)hist->GetFunction("gaus");
            pld->second.mean = fit->GetParameter(1);
            pld->second.meanError = fit->GetParError(1);
            pld->second.sigma = fit->GetParameter(2);
            pld->second.sigmaError = fit->GetParError(2);
            pld->second.amplitude = fit->GetParameter(0);
        } 
        else { // fit failed
            cout << "Warning: Gaussian fit of residuals in ROI ";
            cout << "around xray point (" << x << ", " << y;
            cout << ") on layer " << pld->first; 
            cout << " failed (CombinedData::FitGaussian).\n\n";
        }
    } 
    return;    
}

void CombinedData::CalculateMeanDifference() {
    meanDiff = layerData.at(la).mean - layerData.at(lb).mean;
    meanDiffError = sqrt(pow(layerData.at(la).meanError,2) +
                         pow(layerData.at(lb).meanError,2));
    return;
}

void CombinedData::AppendCombinedDataToTable(ofstream &f) {
    f << xPtIndex << "," << x << "," << y << "," << la << "," << lb;
    f << "," << layerData.at(la).offset << ",";
    f << layerData.at(la).offsetError << ",";
    f << layerData.at(lb).offset << ",";
    f << layerData.at(lb).offsetError << "," << offDiff << ",";
    f << offDiffError << ","; 

    // If fit on layerA was successful, print results
    if (layerData.at(la).success == true) {
        f << layerData.at(la).amplitude << ",";
        f << layerData.at(la).mean << ",";
        f << layerData.at(la).meanError <<  ",";
        f << layerData.at(la).sigma << ","; 
        f << layerData.at(la).sigmaError << ",";
    }
    else { // If fit was not successful, write "NA"
        f << "NA,NA,NA,NA,NA,";
    }

    if (layerData.at(lb).success == true) {
        f << layerData.at(lb).amplitude << ",";
        f << layerData.at(lb).mean << ",";
        f << layerData.at(lb).meanError <<  ",";
        f << layerData.at(lb).sigma << ",";
        f << layerData.at(lb).sigmaError << ",";
    }
    else { // If fit was not successful, write "NA"
         f << "NA,NA,NA,NA,NA,";
    }

    // If both fits were successful print residual mean difference
    if ((layerData.at(la).success == true) && 
         (layerData.at(lb).success == true)) {
        f << meanDiff << "," << meanDiffError << '\n';
    }
    else { // If not print NA
        f << "NA,NA\n";
    }

    return;
}
