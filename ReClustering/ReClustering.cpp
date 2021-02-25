#define ReClustering_cxx
#include "ReClustering.h"

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

    // Setup plot manager
    PlotManager* pm = new PlotManager();
    string lStr;
    for (UShort_t i=1; i<=4; i++) {
        lStr = to_string(i);
        /*pm->Add("tgc_analysis_mean_layer_" + lStr,
                "Layer: " + lStr + "TGC Analysis #mu_cluster", )*/
        pm->Add("tgc_analysis_sigma_layer_" + lStr, 
                "Layer: " + lStr + "TGC Analysis #sigma_cluster;No. Clusters;#sigma [mm]", 1600,
                0, 16, myTH1F); // Range based on max strip mult of 5*strip pitch
        pm->Add("reclustering_sigma_layer_" + lStr, 
                "Layer: " + lStr + "Cluster #sigma;No. Clusters;#sigma [mm]", 1600,
                0, 16, myTH1F);
    }

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
    for (auto activeBranchName : {"eventnumber", "trackX", "trackYGaussian", "trackYWeighted", 
                                  "sumPDO", "rms", "sigma", "pdo_strip", "posCH", 
                                  "relClPosGaussian", "clSize", "isSaturated"}) {
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

    pm->Add(reclustered, myTTree);

    // Add new branches
    // These contain the fit parameters calculated from redoing the cluster fit
    map <UShort_t, Double_t> amplitude;
    map <UShort_t, Double_t> amplitudeError;
    map <UShort_t, Double_t> mean;
    map <UShort_t, Double_t> meanError;
    map <UShort_t, Double_t> newSigma;
    map <UShort_t, Double_t> newSigmaError;
    map <UShort_t, Int_t>  ndf;
    map <UShort_t, Int_t> chi2;

    reclustered->Branch("amplitude", &amplitude);
    reclustered->Branch("amplitudeError", &amplitudeError);
    reclustered->Branch("mean", &mean);
    reclustered->Branch("meanError", &meanError);
    reclustered->Branch("newSigma");
    reclustered->Branch("newSigmaError");
    reclustered->Branch("ndf", &ndf);
    reclustered->Branch("chi2", &chi2);

    Int_t nEntries = reclustered->GetEntries();

    // Setup the vars you need from tree
    map <UShort_t, Double_t> trackYGaussian;
    map <UShort_t, Double_t>* trackYGaussianPtr;
    trackYGaussianPtr = &trackYGaussian;

    map <UShort_t, Double_t> trackYWeighted;
    map <UShort_t, Double_t>* trackYWeightedPtr;
    trackYWeightedPtr = &trackYWeighted;

    map <UShort_t, Double_t> sumPDO;
    map <UShort_t, Double_t>* sumPDOPtr;
    sumPDOPtr = &sumPDO;

    map <UShort_t, Double_t> rms;
    map <UShort_t, Double_t>* rmsPtr;
    rmsPtr = &rms;

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
    reclustered->SetBranchAddress("trackYWeighted", &trackYWeightedPtr);
    reclustered->SetBranchAddress("sumPDO", &sumPDOPtr);
    reclustered->SetBranchAddress("rms", &rmsPtr);
    reclustered->SetBranchAddress("sigma", &sigmaPtr);
    reclustered->SetBranchAddress("pdo_strip", &pdoStripPtr);
    reclustered->SetBranchAddress("posCH", &posCHPtr);
    UShort_t layer;
    vector<Double_t> pos;
    vector<Double_t> pdo;
    Int_t numFits = 0;
    Int_t failedFitCount = 0;

    // for (Int_t i=0 i<nEntries; i++) {
    for (Int_t i=0; i<1; i++) {
        // Get entry
        reclustered->GetEntry();
        // Clear the leaves to output
        amplitude.clear(); amplitudeError.clear();
        mean.clear(); meanError.clear();
        newSigma.clear(); newSigmaError.clear();
        ndf.clear(); chi2.clear();
        // Assumes that posCH and pdoStrip are same size
        for (auto val=posCH.begin(); val!=posCH.end(); val++) {
            layer = val->first;
            pos = val->second;
            pdo = pdoStrip.at(layer);
            // Set initial parameter guesses for Gaussian fit
            // Same guesses as in CosmicsAnalysis
            GausFitInfo fitInfo;
            fitInfo.A = sumPDO.at(layer)/(sqrt(2*TMath::Pi())*rms.at(layer));
            fitInfo.mean = trackYWeighted.at(layer);
            fitInfo.sigma = rms.at(layer);
            // Do fit
            DoGausFitMinuit(pos, pdo, fitInfo, false);
            // Store fit parameters in branches
            amplitude[layer] = fitInfo.A;
            amplitudeError[layer] = fitInfo.Aerr;
            mean[layer] = fitInfo.mean;
            meanError[layer] = fitInfo.meanErr;
            newSigma[layer] = fitInfo.sigma;
            newSigmaError[layer] = fitInfo.sigmaErr;
            ndf[layer] = fitInfo.NDF;
            chi2[layer] = fitInfo.chi2;

            // May need to add a check here in case fit fails.
            if (fitInfo.fitResult==false) failedFitCount++; // Keep tally of # of fits that fail
            numFits++; 
        }
        reclustered->Fill();
    }
    // reclustered->Write(); // Write tree to output file
    cout << "Notice: " << failedFitCount << " of " << numFits << " fits failed\n";
    
    pm->Write(outFile);

    delete pm;
    caFile->Close();
    delete caFile;
    outFile->Close();
    delete outFile;
    return 0;
}

/*void makeReClusteringPlots(TTree* rTree) {
    TCanvas* c = new TCanvas();
    rTree->Draw("")
    return;
}*/
