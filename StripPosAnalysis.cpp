// Main function of strip position analysis
// Author: Lia Formenti    Date: 2020-04-23
// lia.formenti@mail.mcgill.ca

// C++ includes
#include <iostream>

// Root includes
#include <TFile.h>
#include <TTree.h>
#include <TSystem.h>

// My includes
#include "RunAnalysis.h"
#include "GetAnalysisInfo.h"

using namespace std;

int main(int argc, char* argv[]) {
    
    // Check file
    if (argc < 2)
        throw runtime_error("Please specify the file to analyse.");
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
    GetAnalysisInfo(cosmicsAnalysis);

    RunAnalysis(*tracksTree);

    cosmicsAnalysis->Close();
    return 0;
}
