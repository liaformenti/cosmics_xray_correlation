// Main function of strip position analysis
// Author: Lia Formenti    Date: 2020-04-23
// lia.formenti@mail.mcgill.ca

// C++ includes
#include <iostream>

// Root includes
#include <TFile.h>
#include <TTree.h>
#include <TSystem.h>

using namespace std;

int main(int argc, char* argv[]) {
    // Check file
    if (argc < 2) {
        cout << "Please specify the file to analyse." << endl;
        return -1;
    }

    if (gSystem->AccessPathName(argv[1])){
        cout << "File does not exist." << endl;
        return -1;
    }
    // Open file
    TFile cosmicsAnalysis(argv[1]);
    if (cosmicsAnalysis.IsZombie()) {
        cout << "Error opening file." << endl;
        return -1;
    }

    // Check TTree
    if (!cosmicsAnalysis.GetListOfKeys()->Contains("tracks")) {
        cout << "No tracks TTree. Are you sure you have the right file?" << endl;
        return -1;
    }

    TTree* tracksTree = (TTree*)cosmicsAnalysis.Get("tracks");
    tracksTree->Print();

    cosmicsAnalysis.Close();
    return 0;
}
