#define RunAnalysis_cxx
#include "RunAnalysis.h"

using namespace std;

void RunAnalysis(TTree &trksTree, AnalysisInfo* cosmicsInfo, PlotManager* pm, DetectorGeometry* g, InputInfo* myInfo) {

    // Declaration 
    Int_t nEntries;
    Int_t eventnumber;

    map<UShort_t, Double_t> trackX;
    map<UShort_t, Double_t>* trackXPtr;
    trackXPtr = &trackX;
    
    // Uncertainty in x hits
    // Initialized in loop to only have entries on layers with hits
    map<UShort_t, Double_t> uncertX;
    
    map<UShort_t, Double_t> trackYGaussian;
    map<UShort_t, Double_t>* trackYGaussianPtr;
    trackYGaussianPtr = &trackYGaussian;

    // Uncertainty on y position (sigma of strip cluster gaussian fit)
    map<UShort_t, Double_t> sigma;
    map<UShort_t, Double_t>* sigmaPtr;
    sigmaPtr = &sigma;

    //Initialization
    trksTree.SetBranchAddress("eventnumber", &eventnumber);
    trksTree.SetBranchAddress("trackX", &trackXPtr);
    trksTree.SetBranchAddress("trackYGaussian", &trackYGaussianPtr);
    trksTree.SetBranchAddress("sigma", &sigmaPtr);

    nEntries = trksTree.GetEntries();

    UShort_t lc = 0; UShort_t ld = 0;

    // Vector to store calculated residuals
    vector<Residual> residuals;

    cout << "Tracking and calculating residuals...\n\n";

    // initializeUncertaintyHistograms(pm);
    // Replace i<x=nEntries eventually
    // 3 events ensures you get one that passes cut 
    // with testCA_qs3p7.root with L3 and L4 fixed
    for (Int_t i=0; i<nEntries; i++) {
        trksTree.GetEntry(i);
        // Uncertainty in x is width of wire group / sqrt(12)
        // Assumes uniform position distribution of hit across group
        // Some edge wires groups have less wires - later correction
        for (auto itX=trackX.begin(); itX!=trackX.end(); itX++)
            uncertX[itX->first] = 1.8*20/sqrt(12.0); 

        // for each permutation of two layers
        // la < lb and treated first always
        // for (Int_t la=1; la<=4; la++) {
        // Given data right now only do fixed layers 1,4
        for (UShort_t la=1; la<=1; la++) {
            // for (Int_t lb=(la+1); lb<=4; lb++) {
            for (UShort_t lb=4; lb<=4; lb++) {

                if (MissingHitsOnFixedLayers(la, lb, 
                   trackX, trackYGaussian))
                   continue;

                getOtherLayers(la, lb, &lc, &ld);
                Tracking myTrack(g, pm, trackX, uncertX, trackYGaussian, sigma, la, lb);

                myTrack.Fit();
                // Check if hit exists on unfixed layers
                // If so evaluate and calculate residual
                Residual res;
                if (myTrack.hitsY.find(lc) != myTrack.hitsY.end()) {
                    myTrack.EvaluateAt(lc);
                    // pm->Fill("uncertainty_y_evaluations_" + Combination(lc, la, lb).String(), myTrack.fitYUncerts.at(lc));
                    res = Residual(myTrack, lc);
                    residuals.push_back(res);
                }
                if (myTrack.hitsY.find(ld) != myTrack.hitsY.end()) {
                    myTrack.EvaluateAt(ld);
                    // pm->Fill("uncertainty_y_evaluations_" + Combination(ld, la, lb).String(), myTrack.fitYUncerts.at(ld));
                    res = Residual(myTrack, ld);
                    residuals.push_back(res);
                }
                // Plot linear fit
                /*if (i==0) {
                    myTrack.PlotFit("fits_event_" + to_string(eventnumber) + "_fixed_layers_" + to_string(la) + "_" + to_string(lb) + ".pdf");
                }*/
            } // end ld loop
        } // end lc loop
        // Count iterations
        /*if (i%1000==0) {
            cout << "Iteration " << i << " of " <<  nEntries << '\n';
        }*/
    } // end event loop

    cout << "Analyzing results...\n\n";

    // printUncertaintyHistograms(pm);

    // Get xray data
    XRayData xData("results.db", cosmicsInfo, myInfo);
    // Will hold pairs of layers to take offset difference
    vector<pair<UShort_t, UShort_t>> layers;
    Int_t xWidth = 37;
    Int_t yWidth = 20;
    UShort_t la, lb;
    CombinedData data;
    string nameBase;
    TH1I* hist;
    TCanvas * c = new TCanvas();
    c->Print((myInfo->outpath + myInfo->quadname + "_fits_per_xray_pt.pdf[").c_str());
    ofstream tableOut;
    tableOut.open(myInfo->outpath + myInfo->quadname + "_compare_mean_and_offset_differences_table.csv");
    for (Int_t i = 0; i<xData.xnoms.size(); i++) {
    // Just looking at a single xray pt:
    // for (UShort_t i = 3; i<4; i++) {
        cout << "Check num: " << xData.nums.at(i) << ' ' << i << '\n';
        layers = xData.GetDiffCombos(xData.offsets.at(i));
        for (auto ls=layers.begin(); ls!=layers.end(); ls++) {
            data = CombinedData(xWidth, yWidth, i, xData.xnoms.at(i), 
                xData.ynoms.at(i), ls->first, ls->second, 
                xData.offsets.at(i).at(ls->first),
                xData.offsets.at(i).at(ls->second),
                &residuals, g, pm);
            getOtherLayers(ls->first, ls->second, &la, &lb);
            nameBase = "residuals_around_xray_pt_" + to_string(i);
            nameBase += "_width_in_x_" + to_string(xWidth);
            nameBase += "_width_in_y_" + to_string(yWidth) + "_";
            hist = (TH1I*)pm->GetTH1I(nameBase + Combination(ls->first, la, lb).String());
            hist->Draw();             
            c->Print((myInfo->outpath + myInfo->quadname + "_fits_per_xray_pt.pdf").c_str());
            hist = (TH1I*)pm->GetTH1I(nameBase + Combination(ls->second, la, lb).String()); 
            hist->Draw();             
            c->Print((myInfo->outpath + myInfo->quadname + "_fits_per_xray_pt.pdf").c_str());
            // data.histC.Draw();
            // data.fitC.Draw("Same");
            // c->Print((myInfo->outpath + myInfo->quadname + "_fits_per_xray_pt.pdf").c_str());
            // data.histD.Draw();
            // data.fitD.Draw("Same");
            // c->Print((myInfo->outpath + myInfo->quadname + "_fits_per_xray_pt.pdf").c_str());
            data.PrintClassDataToFile(tableOut);
        }
        cout << '\n';
    }
    c->Print((myInfo->outpath + myInfo->quadname + "_fits_per_xray_pt.pdf]").c_str());
    tableOut.close();
    /*data.PlotPositions();
    data.WriteOutXRayData();

    // Create ResPlots for XRayData
    Binning xRayBins(&data, 36, 20, g);
    // Make xray data binned plots
    ResPlots xRayPlots(&residuals, &xRayBins, xRayBins.name, cosmicsInfo, g, pm, myInfo);
    xRayPlots.CreateNumEntriesTH2Is();
    xRayPlots.CreatePosBinnedResPlots();
    xRayPlots.CreatePosBinnedFitResultTH2Fs();
    xRayPlots.PrintNumEntriesTH2Is(myInfo->outpath + myInfo->quadname + "_3100V_num_entries_binning_" + xRayBins.name + ".pdf");
    xRayPlots.PrintPosBinnedResPlots(myInfo->outpath + myInfo->quadname + "_3100V_residual_fits_binning_" + xRayBins.name + ".pdf");
    xRayPlots.PrintPosBinnedFitResultTH2Fs(myInfo->outpath + myInfo->quadname + "_3100V_fit_results_binning_" + xRayBins.name + ".pdf");*/

    cout << "Finishing analysis...\n\n";
    delete c;
    return;
}

Bool_t MissingHitsOnFixedLayers(UShort_t fixed1, UShort_t fixed2, map<UShort_t, Double_t> &xTrack, map<UShort_t, Double_t> &yTrack) {
    // If one or more of the fixed layers is missing a hit in x or y,
    // return true, else return false
    Bool_t missingHit =  ( ! ( (xTrack.find(fixed1) != xTrack.end()) && 
                               (xTrack.find(fixed2) != xTrack.end()) && 
                               (yTrack.find(fixed1) != yTrack.end()) && 
                               (yTrack.find(fixed2) != yTrack.end()) ) );
    return missingHit;
}

void initializeUncertaintyHistograms(PlotManager* pm) {
    vector<Combination> combVec = combinationVector();
    string headerY = "uncertainty_y_evaluations_";
    for (auto v=combVec.begin(); v!=combVec.end(); v++) {
        // Just try the bin number and limits for now
        pm->Add(headerY + v->String(), headerY + v->String(),
                60, 0, 20, myTH1F);
    }
}

void printUncertaintyHistograms(PlotManager* pm) {
    TCanvas *c = new TCanvas();
    c->Print("y_evaluation_uncertainties.pdf[");
    vector<Combination> combVec = combinationVector();
    for (auto comb=combVec.begin(); comb!=combVec.end(); comb++) {
        TH1F* uyhist = (TH1F*)pm->GetTH1F("uncertainty_y_evaluations_" + comb->String());
        uyhist->Draw();
        c->Print("y_evaluation_uncertainties.pdf");
    }
    c->Print("y_evaluation_uncertainties.pdf]");
    delete c;
}
