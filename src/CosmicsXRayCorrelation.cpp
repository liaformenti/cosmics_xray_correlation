// Main function of strip position analysis
// Author: Lia Formenti    Date: 2020-04-23
// lia.formenti@mail.mcgill.ca

// C++ includes
#include <iostream>

// Root includes
#include <TROOT.h>
#include <TStyle.h>
#include <TFile.h>
#include <TTree.h>
#include <TSystem.h>

//tgc_analysis includes
#include "DetectorGeometryTools.h"
#include "PlotManager.h"
#include "AnalysisStyle.h"

// My includes
// #include "RunAnalysis.h"
#include "CosmicsRetracking.h"
#include "GetAnalysisInfo.h"
#include "XRayData.h"
#include "XRayRetracking.h"
#include "Residual.h"
#include "CompareData.h"
#include "Binning.h"
#include "ResPlots.h"

using namespace std;

int main(int argc, char* argv[]) {

    // Set style
    SetAnalysisStyle();
    // cout << "You commented out SetAnalysisStyle\n\n";
    // Add the Formenti flair
    gStyle->SetOptStat("e");
    gStyle->SetOptFit(1);
    gROOT->ForceStyle();
    gErrorIgnoreLevel = kWarning;
    
    // Check arguments
    // This should be upgraded to a config file and better cmd line argument format.
    // Also right now you must provide a tag. 
    if (argc != 6) 
        throw runtime_error("Usage example: ./CosmicsXRayCorrelation input_reclustering.root QUADNAME input_xray.db outpath/ tag_\n Quadruplet name format, eg. QL2P06.\n\n");

    // Check quad name - need name to compare with xray data
    if (argv[2][0] != 'Q') {
        throw runtime_error("Second argument must be quadruplet name in format, eg. QL2P06");
    } 
   
    // Check that database exists
    if (gSystem->AccessPathName(argv[3]))
        throw runtime_error("Database does not exist.");

    // Check that output directory exists
    if (gSystem->AccessPathName(argv[4]))
        throw runtime_error("Output directory does not exist.");

    // Fill InputInfo struct
    InputInfo myInfo(argv[2], argv[3], argv[4], argv[5]);
    // cout << myInfo.quadname << ' ' << myInfo.database << ' ' << myInfo.outpath << ' ' << myInfo.tag << "\n\n";

    // Check input file
    if (gSystem->AccessPathName(argv[1]))
        throw runtime_error("File does not exist.");

    // Open input file
    TFile* f = new TFile(argv[1], "READ");
    if (f->IsZombie())
        throw runtime_error("Error opening file.");

    // Check TTree
    if (!f->GetListOfKeys()->Contains("tracks"))
        throw runtime_error("No tracks TTree. Are you sure you have the right file?");

    // Get TTree
    TTree* tracksTree = (TTree*)f->Get("tracks");

    // Get AnalysisInfo object, error handling done in fcn
    AnalysisInfo* cosmicsInfo = GetAnalysisInfo(f);
    if (cosmicsInfo == nullptr)
       throw runtime_error("Error getting AnalysisInfo object, in function GetAnalysisInfo.\n\n"); 
   
    // Get detector geometry
    DetectorGeometry *geom = DetectorGeometryTools::GetDetectorGeometry(cosmicsInfo->detectortype);

    PlotManager* plotManager = new PlotManager();    

    // Get xray data
    // cout << "Not getting x-ray data.\n\n";
    cout << "Getting x-ray data...\n\n";
    XRayData xData(cosmicsInfo, &myInfo, plotManager);
    xData.WriteOutXRayData();
    xData.PlotAverageBeamPositions();

    // cout << "Not re-tracking x-ray data\n\n";
    cout << "Re-tracking x-ray data...\n\n";
    XRayRetracking xrayTracks(&xData, cosmicsInfo, &myInfo, plotManager, geom);
    xrayTracks.Retrack();
    // Temporary way to printout xray residuals
    ofstream of;
    of.open(myInfo.outpath + myInfo.quadname + "_xray_residuals.txt");
    of << "layer,fixed_layer_a, fixed_layer_b, x, y, residual, residual error, tag, mm\n";
    for (auto r=xrayTracks.residuals.begin(); r!=xrayTracks.residuals.end(); r++){
        of << r->l << "," << r->la << "," << r->lb << "," << r->x << "," << r->y << ",";
        of << r->res << "," << r->resErr << "," << r->tag << "\n"; 
    }
    of.close();

    // cout << "Not retracking cosmics.\n\n";
    cout << "Re-tracking cosmics...\n\n";
    CosmicsRetracking cosmicTracks(tracksTree, cosmicsInfo, &myInfo, plotManager, geom);
    cosmicTracks.Retrack();
    cosmicTracks.PrintTrackAngleHistograms();
    cosmicTracks.PrintTrackUncertaintyHistograms();
    cosmicTracks.PrintResidualUncertaintyHistograms();
    
    // cout << "Not comparing cosmics and x-ray data.\n\n";
    cout << "Comparing cosmics and x-ray data...\n\n";
    CompareData comp(100, 100, &xrayTracks.residuals, &cosmicTracks.residuals, &myInfo, plotManager, 
                     geom);
    comp.DoComparison();
    comp.MakeScatterPlot();
    comp.OutputLocalDataToCSV();

    // TH2s
    Binning widthBins(100, 100, geom);
    ResPlots th2s(&cosmicTracks.residuals, &widthBins, widthBins.name, cosmicsInfo, geom, plotManager, &myInfo);
    th2s.CreateNumEntriesTH2Is();
    th2s.CreatePosBinnedResPlots();
    th2s.CreatePosBinnedFitResultTH2Fs();
    th2s.CreateResidualDistributions();
    th2s.CreateDNLPlots();
    th2s.PrintNumEntriesTH2Is(myInfo.outpath + myInfo.tag + myInfo.quadname + "_num_entries_binning_" + widthBins.name + ".pdf");
    th2s.PrintPosBinnedResPlots(myInfo.outpath + myInfo.tag + myInfo.quadname + "_residual_fits_binning_" + widthBins.name + ".pdf");
    th2s.PrintPosBinnedFitResultTH2Fs();
    th2s.PrintResidualDistributions(myInfo.outpath + myInfo.tag + myInfo.quadname + "_residual_distributions_" + widthBins.name + ".pdf");
    th2s.PrintDNLPlots(myInfo.outpath + myInfo.tag + myInfo.quadname + "_residual_vs_yrel.pdf");

    cout << "Finishing up...\n\n"; 
    // Dump all objects to root file
    TFile* outRoot = new TFile(
            (myInfo.outpath + myInfo.tag + "cosmics_xray_correlation.root").c_str(), "RECREATE");
    plotManager->Write(outRoot);
    delete outRoot;
    delete plotManager;
    f->Close();
    delete f;
    return 0;
}
