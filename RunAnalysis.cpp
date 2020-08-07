#define RunAnalysis_cxx
#include "RunAnalysis.h"

using namespace std;

void RunAnalysis(TTree &trksTree, AnalysisInfo* cosmicsInfo, PlotManager* pm, DetectorGeometry* g, InputInfo* myInfo) {

    // Declaration 
    Int_t nEntries;
    Int_t eventnumber;

    Bool_t makePlots = false; // Whether to make plots in tracking loop

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

    if (makePlots) initializeUncertaintyHistograms(pm);
    if (makePlots) initializeTrackAngleHistograms(pm);
    for (Int_t i=0; i<nEntries; i++) {
        trksTree.GetEntry(i);
        // Uncertainty in x is width of wire group / sqrt(12)
        // Assumes uniform position distribution of hit across group
        // Some edge wires groups have less wires - later correction
        for (auto itX=trackX.begin(); itX!=trackX.end(); itX++)
            uncertX[itX->first] = 1.8*20/sqrt(12.0); 

        // for each permutation of two layers
        // la < lb and treated first always
        for (UShort_t la=1; la<=4; la++) {
        // for (UShort_t la=1; la <=1; la++) {
            for (UShort_t lb=(la+1); lb<=4; lb++) {
            // for (UShort_t lb=4; lb<=4; lb++) {

                if (MissingHitsOnFixedLayers(la, lb, 
                   trackX, trackYGaussian))
                   continue;

                getOtherLayers(la, lb, &lc, &ld);
                Tracking myTrack(g, pm, trackX, uncertX, trackYGaussian, sigma, la, lb);

                myTrack.Fit();
                if (makePlots) {
                    pm->Fill("track_angle_x_fixed_layers_" + to_string(la) + to_string(lb), tan(myTrack.resultX->Value(1)));
                    pm->Fill("track_angle_y_fixed_layers_" + to_string(la) + to_string(lb), tan(myTrack.resultY->Value(1)));
                }
                // Check if hit exists on unfixed layers
                // If so evaluate and calculate residual
                Residual res;
                if (myTrack.hitsY.find(lc) != myTrack.hitsY.end()) {
                    myTrack.EvaluateAt(lc);
                    if (makePlots) pm->Fill("uncertainty_y_evaluations_" + Combination(lc, la, lb).String(), myTrack.fitYUncerts.at(lc));
                    res = Residual(myTrack, lc);
                    residuals.push_back(res);
                }
                if (myTrack.hitsY.find(ld) != myTrack.hitsY.end()) {
                    myTrack.EvaluateAt(ld);
                    if (makePlots) pm->Fill("uncertainty_y_evaluations_" + Combination(ld, la, lb).String(), myTrack.fitYUncerts.at(ld));
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
    
    if (makePlots) printUncertaintyHistograms(pm, myInfo);
    if (makePlots) printTrackAngleHistograms(pm, myInfo);

    // Get xray data
    XRayData xData("results.db", cosmicsInfo, myInfo, pm);
    xData.PlotPositions();
    xData.WriteOutXRayData();

    // Main offset vs mean difference analysis
    // Will hold pairs of layers to take offset difference
    vector<pair<UShort_t, UShort_t>> layers;
    Int_t xWidth = 40;
    Int_t yWidth = 40;
    CombinedData data;
    TH1I* hist;
    TCanvas * c = new TCanvas();
    string drawOutFileName = myInfo->outpath + myInfo->quadname + "_fits_per_xray_pt_xROI_" + to_string(xWidth) + "mm_width_yROI_" + to_string(yWidth) + "mm_width.pdf";
    c->Print((drawOutFileName + "[").c_str());
    ofstream tableOut;
    string tableOutFileName = myInfo->outpath + myInfo->quadname + "_compare_mean_and_offset_differences_table_xROI_" + to_string(xWidth) + "mm_width_yROI_" + to_string(yWidth) + "mm_width.csv";
    tableOut.open(tableOutFileName);

    for (auto xrayPt=xData.pointVec.begin(); 
              xrayPt!=xData.pointVec.end(); xrayPt++) {
        // Get pairs of layers on which there is data to do differences
        layers = xrayPt->GetDiffCombos();

        for (auto lp=layers.begin(); lp!=layers.end(); lp++) {
            // Process xray and residuals data
            data = CombinedData(*xrayPt, lp->first, lp->second, 
                                &residuals, g, pm);
            data.DefineRectangularROI(xWidth, yWidth);
            data.FillROIsWithResiduals();
            data.CreateResidualHistograms();
            data.FitGaussian();
            data.CalculateMeanDifference();
            data.AppendCombinedDataToTable(tableOut);

            // Draw layerA histogram
            hist = (TH1I*)pm->Get(data.layerData.at(lp->first).histName);
            if (hist->GetEntries() != 0) {
                hist->Draw();
                c->Print(drawOutFileName.c_str());
            }
            // Draw layerB histogram
            hist = (TH1I*)pm->Get(data.layerData.at(lp->second).histName);
            if (hist->GetEntries() != 0) {
                hist->Draw();
                c->Print(drawOutFileName.c_str());
            }

        } // end layers loop
    } // end xray point loop

    c->Print((drawOutFileName + "]").c_str());

    delete c;
    tableOut.close();

    /*
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
        pm->Add(headerY + v->String(), "Layer: " + to_string(v->layer) + ", Fixed layers: " + to_string(v->fixed1) + to_string(v->fixed2) + " y-track fit uncertainty;Uncertainty [mm];Tracks;", 60, 0, 20, myTH1F);
    }
}

void printUncertaintyHistograms(PlotManager* pm, InputInfo* myInf) {
    TCanvas *c = new TCanvas();
    string outName = myInf->outpath + "y_evaluation_uncertainties.pdf";
    c->Print((outName + "[").c_str());
    vector<Combination> combVec = combinationVector();
    for (auto comb=combVec.begin(); comb!=combVec.end(); comb++) {
        TH1F* uyhist = (TH1F*)pm->GetTH1F("uncertainty_y_evaluations_" + comb->String());
        uyhist->Draw();
        c->Print(outName.c_str());
    }
    c->Print((outName + "]").c_str());
    delete c;
}

void initializeTrackAngleHistograms(PlotManager* pm) {
    // One hist for each fixed layer combination for x and for y
  string name, title;
  string coord[2] = {"x", "y"};
  for (UShort_t la=1; la<=4; la++) {
    for (UShort_t lb=(la+1); lb<=4; lb++) {
        for (UShort_t i=0; i<2; i++) {
         name = "track_angle_" + coord[i] + "_fixed_layers_"; 
         name += to_string(la) + to_string(lb);             
         title = "Fixed layers: " + to_string(la) + to_string(lb);
         title += " " + coord[i] + "-track angles;Angle [rads];Tracks";
         pm->Add(name, title, 120, -3.14/2, 3.14/2, myTH1I);
       }
     }
   }
   return;
}

void printTrackAngleHistograms(PlotManager* pm, InputInfo* myInf) {
  TCanvas *c = new TCanvas();
  string outName = myInf->outpath + "track_angle_hists.pdf";
  c->Print((outName + "[").c_str());
  string coord[2] = {"x", "y"};

  for (UShort_t i=0; i<2; i++) {
    for (UShort_t la=1; la<=4; la++) {
      for (UShort_t lb=(la+1); lb<=4; lb++) {
        TH1I* anghist = (TH1I*)pm->GetTH1I("track_angle_" + coord[i] + "_fixed_layers_" + to_string(la) + to_string(lb));             
        anghist->Draw();
        c->Print(outName.c_str());
      }
    }
  }
  c->Print((outName + "]").c_str());
  delete c;
}
