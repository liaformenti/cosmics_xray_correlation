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
    // SetAnalysisStyle();
    cout << "You commented out SetAnalysisStyle\n";
    // Check file
    if ((argc < 3) || (argc > 4))
        throw runtime_error("Please specify the file to analyse, the quadruplet name, and if desired a single string (nospaces) with futher information you would like to include in output file names.");

    // Check quad name - need name to compare with xray data
    if (argv[2][0] != 'Q') {
        throw runtime_error("The second argument should be the quadruplet name, in the format, eg. QS3P07");
    } 
   
    // Fill DataInfo struct
    DataInfo about;
    about.quadname = argv[2];
    if (argc == 4) {
        // An otherInfo string was included, for use in filenaming
        about.otherInfo = argv[3];
    }
    
    if (gSystem->AccessPathName(argv[1]))
        throw runtime_error("File does not exist.");

    // Open file
    TFile* cosmicsAnalysis = new TFile(argv[1], "READ");
    if (cosmicsAnalysis->IsZombie())
        throw runtime_error("Error opening file.");

    // Check TTree
    if (!cosmicsAnalysis->GetListOfKeys()->Contains("tracks"))
        throw runtime_error("No tracks TTree. Are you sure you have the right file?");

    // Get TTree
    TTree* tracksTree = (TTree*)cosmicsAnalysis->Get("tracks");

    // Get AnalysisInfo object, error handling done in fcn
    AnalysisInfo* analysisInfo = GetAnalysisInfo(cosmicsAnalysis);
    if (analysisInfo == nullptr)
       throw runtime_error("Error getting AnalysisInfo object, in function GetAnalysisInfo"); 

    PlotManager* plotManager = new PlotManager();    
    // Get detector geometry
    DetectorGeometry *geom = DetectorGeometryTools::GetDetectorGeometry(analysisInfo->detectortype);

    RunAnalysis(*tracksTree, analysisInfo, plotManager, geom);
    // cout << "You commented out call to RunAnalysis\n";

    delete plotManager;
    cosmicsAnalysis->Close();
    return 0;
}
