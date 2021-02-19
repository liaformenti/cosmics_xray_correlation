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

    // Open input file
    TFile* caFile = new TFile(argv[1], "READ");
    if (caFile->IsZombie())
        throw runtime_error("Error opening CosmicsAnalysis.root file.\n\n");

    // Check input file has tracks TTree
    if (!caFile->GetListOfKeys()->Contains("tracks"))
        throw runtime_error("No tracks TTree in CosmicsAnalysis.root file. Cannot perform analysis.\n\n");

    // Get TTree
    TTree* tracks = (TTree*)caFile->Get("tracks");

    // Deactivate all branches
    tracks->SetBranchStatus("*", 0);

    // Activate the branches you want to copy to output tree
    for (auto activeBranchName : {"eventnumber", "trackX", "trackYGaussian", "sigma", "pdo_strip",
                                  "posCH", "relClPosGaussian", "clSize", "isSaturated"}) {
        tracks->SetBranchStatus(activeBranchName, 1);
    }

    // Open output file
    // ROOT complains if you mix accessing a tree from one root file and writing to another
    // so opening the output file must happen here.

    string outpath = argv[2];
    TFile* outFile = new TFile((outpath + tag + "reclustering.root").c_str(), "RECREATE");
    if (outFile->IsZombie())
        throw runtime_error("Error opening output file.\n\n");

    TTree* reclustered = tracks->CloneTree();

    // Add new branches
    // These contain the fit parameters calculated from redoing the cluster fit
    map <UShort_t, Double_t> amplitude;
    map <UShort_t, Double_t> amplitudeError;
    map <UShort_t, Double_t> mean;
    map <UShort_t, Double_t> meanError;
    map <UShort_t, Double_t> newsigma;
    map <UShort_t, Double_t> newsigmaError;
    map <UShort_t, Int_t>  ndf;
    map <UShort_t, Int_t> chi2;

    reclustered->Branch("amplitude", &amplitude);
    reclustered->Branch("amplitudeError", &amplitudeError);
    reclustered->Branch("mean", &mean);
    reclustered->Branch("meanError", &meanError);
    reclustered->Branch("newsigma");
    reclustered->Branch("newsigmaError");
    reclustered->Branch("ndf", &ndf);
    reclustered->Branch("chi2", &chi2);

    Int_t nEntries = reclustered->GetEntries();

    // Setup the vars you need from tree
    map <UShort_t, Double_t> trackYGaussian;
    map <UShort_t, Double_t>* trackYGaussianPtr;
    trackYGaussianPtr = &trackYGaussian;

    map<UShort_t, Double_t> sigma;
    map <UShort_t, Double_t>* sigmaPtr;
    sigmaPtr = &sigma;

    map <UShort_t, vector<Double_t>> pdoStrip;
    map <UShort_t, vector<Double_t>>* pdoStripPtr;
    pdoStripPtr = &pdoStrip;

    map <UShort_t, vector<Double_t>> posCH;
    map <UShort_t, vector<Double_t>>* posCHPtr;
    posCHPtr = &posCH;

    reclustered->SetBranchAddress("trackYGaussian", &trackYGaussianPtr);
    reclustered->SetBranchAddress("sigma", &sigmaPtr);
    reclustered->SetBranchAddress("pdo_strip", &pdoStripPtr);
    reclustered->SetBranchAddress("posCH", &posCHPtr);
    // for (Int_t i=0; i<nEntries; i++) {
    for (Int_t i=0; i<3; i++) {
        reclustered->GetEntry();
    }
    caFile->Close();
    delete caFile;
    outFile->Close();
    delete outFile;
    return 0;
}
