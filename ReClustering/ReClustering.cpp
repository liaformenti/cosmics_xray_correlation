//***********************************************************
// ReClustering takes a tracks TTree from CosmicsAnalysis and
// redoes the cluster fit.
// It outputs the cluster parameters in a reformatted TTree
// with more information about the fit than the original, 
// namely, the fit parameter uncertainties.
//
// Author: Lia Formenti
// Date: 2021-02-18
// lia.formenti@mail.mcgill.ca
// ************************************************************

// C++ include
#include <iostream>
#include <vector>
#include <map>

// ROOT includes
#include <TROOT.h>
#include <TSystem.h>
#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TH1F.h>

using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 3) {
        cout << "Usage: ./ReClustering CosmicsAnalysis.root outpath/ [tag_]\n";
        return 0;
    }
    
    // Check input CosmicsAnalysis.root file
    if (gSystem->AccessPathName(argv[1]))
        throw runtime_error("Input file does not exist\n\n");

    // Check output directory
    if (gSystem->AccessPathName(argv[2]))
        throw runtime_error("Output directory does not exist.\n\n");

    // Get tag if provided
    string tag = "";
    if (argc==4)
        tag = argv[3];

    // Open output file
    string outpath = argv[2];
    TFile* outFile = new TFile((outpath + tag + "reclustering.root").c_str(), "RECREATE");
    if (outFile->IsZombie())
        throw runtime_error("Error opening output file.\n\n");

    // Open input file
    TFile* caFile = new TFile(argv[1], "READ");
    if (caFile->IsZombie())
        throw runtime_error("Error opening CosmicsAnalysis.root file.\n\n");

    // Check input file has tracks TTree
    if (!caFile->GetListOfKeys()->Contains("tracks"))
        throw runtime_error("No tracks TTree in CosmicsAnalysis.root file. Cannot perform analysis.\n\n");

    // Get TTree
    TTree* tracks = (TTree*)caFile->Get("tracks");

    caFile->Close();
    delete caFile;
    outFile->Close();
    delete outFile;
    return 0;
}
