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
#include "RunAnalysis.h"
#include "GetAnalysisInfo.h"

using namespace std;

int main(int argc, char* argv[]) {

    // Set style
    SetAnalysisStyle();
    // cout << "You commented out SetAnalysisStyle\n\n";
    
    // Check arguments
    if (argc != 5) 
        throw runtime_error("Please specify the file to analyse, the quadruplet name, a run-identifier tag, and the output directory path.");

    // Check quad name - need name to compare with xray data
    if (argv[2][0] != 'Q') {
        throw runtime_error("The second argument should be the quadruplet name, matching the format, QS3P07");
    } 
   
    // Check that output directory exists
    if (gSystem->AccessPathName(argv[4]))
        throw runtime_error("Output directory does not exist.");

    // Fill InputInfo struct
    InputInfo myInfo(argv[2], argv[4], argv[3]);

    // Check input file
    if (gSystem->AccessPathName(argv[1]))
        throw runtime_error("File does not exist.");

    // Open input file
    TFile* cosmicsAnalysis = new TFile(argv[1], "READ");
    if (cosmicsAnalysis->IsZombie())
        throw runtime_error("Error opening file.");

    // Check TTree
    if (!cosmicsAnalysis->GetListOfKeys()->Contains("tracks"))
        throw runtime_error("No tracks TTree. Are you sure you have the right file?");

    // Get TTree
    TTree* tracksTree = (TTree*)cosmicsAnalysis->Get("tracks");

    // Get AnalysisInfo object, error handling done in fcn
    AnalysisInfo* cosmicsInfo = GetAnalysisInfo(cosmicsAnalysis);
    if (cosmicsInfo == nullptr)
       throw runtime_error("Error getting AnalysisInfo object, in function GetAnalysisInfo."); 
   
    // Get detector geometry
    DetectorGeometry *geom = DetectorGeometryTools::GetDetectorGeometry(cosmicsInfo->detectortype);

    PlotManager* plotManager = new PlotManager();    

    cout << "Running analysis...\n\n";
    RunAnalysis(*tracksTree, cosmicsInfo, plotManager, geom, &myInfo);
    // cout << "You commented out call to RunAnalysis\n\n";

    cout << "Finishing up...\n\n"; 
    delete plotManager;
    cosmicsAnalysis->Close();
    delete cosmicsAnalysis;
    return 0;
}
