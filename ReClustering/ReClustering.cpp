#define ReClustering_cxx
#include "ReClustering.h"

using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cout << "Usage: ./ReClustering -o outdirectory --tag prefix ";
        cout << "--dnlconfig dnlconfigfile CosmicsAnalysis.root [CosmicsAnalysis2.root...]\n";
        return 0;
    }

    Int_t p=1;
    vector<string> inFileName;
    string outpath, dnlConfigFileName, tag;
    outpath = "out/";
    dnlConfigFileName = "";
    tag = "";

    while(p<argc) {
        
        // Set output directory
        if (string(argv[p]) ==  "-o") {
            if (gSystem->AccessPathName(argv[p+1]))
                throw runtime_error("Output directory does not exist.\n\n");
            outpath = string(argv[p+1]);
            outpath += "/";
            p += 2;
        }

        // Set tag
        else if (string(argv[p]) == "--tag") {
            tag = string(argv[p+1]);
            tag += "_";
            p += 2;
        }

        // Set DNL config file name
        else if (string(argv[p]) == "--dnlconfig") {
            if (gSystem->AccessPathName(argv[p+1]))
                throw runtime_error("Config file does not exist.\n\n");
            dnlConfigFileName = argv[p+1];  
            p += 2;
        }

        // You must be at end, get the input file(s)
        else {
            if (gSystem->AccessPathName(argv[p]))
               throw runtime_error("Input file does not exist.\n\n"); 
            if ( find(inFileName.begin(), inFileName.end(), string(argv[p])) != inFileName.end() )
                throw runtime_error("Duplicate input file detected.\n\n");
            cout << string(argv[p]) << '\n';
            inFileName.push_back(string(argv[p]));
            p += 1;
        }
    }
   
    // Open input file
    TFile* caFile = new TFile(argv[1], "READ");
    if (caFile->IsZombie())
        throw runtime_error("Error opening CosmicsAnalysis.root file.\n\n");

    // Check input file has tracks TChain
    if (!caFile->GetListOfKeys()->Contains("tracks"))
        throw runtime_error("No tracks TTree in input file. Cannot perform analysis.\n\n");

    // Get TChain
    TChain* tracks = (TChain*)caFile->Get("tracks");

    // Get AnalysisInfo object
    AnalysisInfo* cInfo = GetAnalysisInfo(caFile);
    if (cInfo == nullptr)
        throw runtime_error("Error getting AnlysisInfo object, in function GetAnalysisInfo.\n\n");

    DetectorGeometry* g = DetectorGeometryTools::GetDetectorGeometry(cInfo->detectortype);
    if (g == nullptr)
        throw runtime_error("Error getting DetectorGeometry object.\n\n");
    
    // Setup plot manager
    PlotManager* pm = new PlotManager();

    // tgc_analysis style
    SetAnalysisStyle();

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

    // string outpath = argv[2];
    TFile* outFile = new TFile((outpath + tag + "reclustering.root").c_str(), "RECREATE");
    if (outFile->IsZombie())
        throw runtime_error("Error opening output file.\n\n");

    cout << "Cloning CosmicsAnalysis tracks tree.\n";
    TTree* reclustered = tracks->CloneTree();
    cout << "Cloned!\n";
    reclustered->SetName("reclustering_tracks");
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

    reclustered->Branch("r_amplitude", &amplitude);
    reclustered->Branch("r_amplitudeError", &amplitudeError);
    reclustered->Branch("r_mean", &mean);
    reclustered->Branch("r_meanError", &meanError);
    reclustered->Branch("r_sigma", &newSigma);
    reclustered->Branch("r_sigmaError", &newSigmaError);
    reclustered->Branch("r_ndf", &ndf);
    reclustered->Branch("r_chi2", &chi2);

    Int_t nEntries = reclustered->GetEntries();

    // Setup the vars you need from tree

    Int_t eventnumber;

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

    reclustered->SetBranchAddress("eventnumber", &eventnumber);
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
    string multStr = "";
    Int_t numFits = 0;
    Int_t failedFitCount = 0;
    // Int_t disagreesWithCosmicsCount = 0;

    pair<Double_t, Double_t> yLims = g->GetModuleLimitsY();
    // Initialize plots
    // All multiplicity
    pm->Add("cosmics_sigma", ";Cosmics #sigma [mm];No. Clusters", 100, 0, 10, myTH1F);
    pm->Add("reclustering_amplitude", ";Amplitude [ADC counts];No. Clusters", 320, 0, 3200, 
            myTH1F);
    pm->Add("reclustering_mean", ";Cluster mean [mm];No.Clusters", yLims.second - yLims.first, 
            yLims.first, yLims.second, myTH1F);
    pm->Add("reclustering_mean_error", ";Cluster mean error [mm];No.Clusters", 50, 0, 0.05, 
            myTH1F);
    pm->Add("reclustering_sigma",";#sigma [mm];No. Clusters", 100, 0, 10, myTH1F);
    // Spec multiplicity
    for (Int_t m=3; m<=8; m++) {
        pm->Add("cosmics_sigma_multiplicity_" + to_string(m), "Cluster size = " + to_string(m) + 
                ";Cosmics #sigma [mm];No. Clusters", 100, 0, 10, myTH1F);
        pm->Add("reclustering_amplitude_multiplicity_" + to_string(m), "Cluster size = " + 
                to_string(m) + ";Amplitude [ADC counts];No. Clusters", 320, 0, 3200, myTH1F);
        pm->Add("reclustering_mean_multiplicity_" + to_string(m), "Cluster size = " + 
                to_string(m) + ";Cluster mean [mm];No.Clusters", yLims.second - yLims.first, 
                yLims.first, yLims.second, myTH1F);
        pm->Add("reclustering_mean_error_multiplicity_" + to_string(m), "Cluster size = " +
                to_string(m) + ";Cluster mean error [mm];No.Clusters", 20, 0, 0.05, myTH1F);
        pm->Add("reclustering_sigma_multiplicity_" + to_string(m), "Cluster size = " +
                to_string(m) + ";#sigma [mm];No. Clusters", 100, 0, 10, myTH1F);
    }
    
    // Make list of plot names for printing to pdf
    vector<string> nameBases{"cosmics_sigma", "reclustering_amplitude","reclustering_mean", 
                             "reclustering_mean_error", "reclustering_sigma"}; 
    vector<string> plotNames;
    for (auto name=nameBases.begin(); name!=nameBases.end(); name++) {
        plotNames.push_back(*name);
        // cout << *name << ' ';
        for (Int_t m=3; m<=8; m++) {
            plotNames.push_back(*name + "_multiplicity_" + to_string(m));
            // cout << *name + "_multiplicity_" + to_string(m) << ' ';
        } 
        // cout << '\n';
    }

    // File for output
    ofstream f;
    f.open(outpath + tag + "sample_cluster_fit.csv");
    cout << "Starting event loop...\n";
    for (Int_t i=0; i<nEntries; i++) {
    // for (Int_t i=0; i<50; i++) {
        // Get entry
        reclustered->GetEntry(i);
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
            /*cout << "Event, layer: " << eventnumber << ' ' << layer << '\n';
            cout << "Positions: ";
            for (auto x=pos.begin(); x!=pos.end(); x++)
                cout << *x << ' ';
            cout << '\n';
            cout << "PDO: ";
            for (auto y=pdo.begin(); y!=pdo.end(); y++)
                cout << *y << ' ';
            cout << '\n';*/
            // Set initial parameter guesses for Minuit2 Gaussian fit
            // Same guesses as in CosmicsAnalysis
            GausFitInfo fitInfo;
            fitInfo.A = sumPDO.at(layer)/(sqrt(2*TMath::Pi())*rms.at(layer));
            fitInfo.mean = trackYWeighted.at(layer);
            fitInfo.sigma = rms.at(layer);
            // Do fit
            DoGausFitMinuit(pos, pdo, fitInfo, false);
            // DoGausFitGuos(pos, pdo, fitInfo, false);
            // Store fit parameters in branches
            amplitude[layer] = fitInfo.A;
            amplitudeError[layer] = fitInfo.Aerr;
            mean[layer] = fitInfo.mean;
            meanError[layer] = fitInfo.meanErr;
            newSigma[layer] = fitInfo.sigma;
            newSigmaError[layer] = fitInfo.sigmaErr;
            ndf[layer] = fitInfo.NDF;
            chi2[layer] = fitInfo.chi2;

            // Fill plots
            if  (fitInfo.fitResult!=0 && pos.size()>=3 && pos.size()<=8) { // If fit is good,
                pm->Fill("cosmics_sigma", sigma.at(layer));
                pm->Fill("reclustering_amplitude", fitInfo.A);
                pm->Fill("reclustering_mean", fitInfo.mean);
                pm->Fill("reclustering_mean_error", fitInfo.meanErr);
                pm->Fill("reclustering_sigma", fitInfo.sigma);
                multStr = to_string(pos.size());
                pm->Fill("cosmics_sigma_multiplicity_" + multStr, sigma.at(layer));
                pm->Fill("reclustering_amplitude_multiplicity_" + multStr, fitInfo.A);
                pm->Fill("reclustering_mean_multiplicity_" + multStr, fitInfo.mean);
                pm->Fill("reclustering_mean_error_multiplicity_" + multStr, fitInfo.meanErr);
                pm->Fill("reclustering_sigma_multiplicity_" + multStr, fitInfo.sigma);
            }
            // Output sample of fits to file
            if (i<50) {
                // event number, multiplicity, tgc_analysis mean, tgc_analysis sigma, fit result,
                // amplitude, amplitude error, mean, mean error, sigma, sigma error, chi2/ndf
                f << eventnumber  << ',' << pos.size() << ',' << trackYGaussian.at(layer);
                f << ',' << sigma.at(layer) << ',' << fitInfo.fitResult << ',' << fitInfo.A;
                f << ',' << fitInfo.Aerr << ',' << fitInfo.mean << ',' << fitInfo.meanErr << ',';
                f << fitInfo.sigma << ',' << fitInfo.sigmaErr << ',' << fitInfo.chi2/fitInfo.NDF;
                f << '\n';
            }

            // May need to add a check here in case fit fails.
            if (fitInfo.fitResult==false) failedFitCount++; // Keep tally of # of fits that fail
            numFits++; 

            // Count the number of times the fit mean disagrees with the cosmics mean.
            /*if ( (fitInfo.mean - fitInfo.meanErr > trackYGaussian.at(layer)) || 
                 (fitInfo.mean + fitInfo.meanErr < trackYGaussian.at(layer)) ) {
                 disagreesWithCosmicsCount++; 
            }*/
            /*if ( abs(fitInfo.mean - trackYGaussian.at(layer))>0.0009 )
                disagreesWithCosmicsCount++;*/
        }
        reclustered->Fill();
    }
    // reclustered->Write(); // Write tree to output file
    cout << "Notice: " << failedFitCount << " of " << numFits << " fits failed\n";
    // cout << "Notice: " << disagreesWithCosmicsCount << " means of " << numFits;
    // cout << " fits diagree with the cosmics mean\n";
    
    f.close();
    // Print plots
    TCanvas* c = new TCanvas();
    c->Print((outpath + tag + "reclustering_plots.pdf[").c_str());
    for (auto name=plotNames.begin(); name!=plotNames.end(); name++) {
        TH1F* h = (TH1F*)pm->GetTH1F(*name);
        h->Draw();
        c->Print((outpath + tag + "reclustering_plots.pdf").c_str());
        c->Clear();
    }
    c->Print((outpath + tag + "reclustering_plots.pdf]").c_str());

    // Copy AnalysisInfo to PlotManager
    pm->Add(cInfo, myAnalysisInfo);
    // Write everything in plot manager to output file
    pm->Write(outFile);
    delete pm;
    caFile->Close();
    delete caFile;
    outFile->Close();
    delete outFile;
    return 0;
}

/*void makeReClusteringPlots(TChain* rTree) {
    TCanvas* c = new TCanvas();
    rTree->Draw("")
    return;
}*/
