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
    Bool_t doDNLCorrection = false;
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
            doDNLCorrection = true;
            cout << "DNL configuration: " << dnlConfigFileName << '\n';
            p += 2;
        }

        // You must be at end, get the input file(s)
        else {
            if (gSystem->AccessPathName(argv[p]))
               throw runtime_error("Input file does not exist.\n\n"); 
            if ( find(inFileName.begin(), inFileName.end(), string(argv[p])) != inFileName.end() )
                throw runtime_error("Duplicate input file detected.\n\n");
            inFileName.push_back(string(argv[p]));
            p += 1;
        }
    }
   
    // Setup plot manager
    PlotManager* pm = new PlotManager();

    // Check the input files and AnalysisInfo objects
    vector<AnalysisInfo*> analysisInfoVec;
    AnalysisInfo* ai;
    string detectortype = "";
    for (auto fileName=inFileName.begin(); fileName!=inFileName.end(); fileName++) {
        TFile *f = TFile::Open((*fileName).c_str());    
        if (f->IsZombie())
            throw runtime_error("Error opening input file.\n\n");
        if (!f->GetListOfKeys()->Contains("tracks"))
            throw runtime_error("Input file does not contain tracks TTree.\n\n");
        ai = GetAnalysisInfo(f);
        if (ai == nullptr)
            throw runtime_error("Error getting AnalysisInfo object.\n\n");
        if (detectortype == "") {
            detectortype = ai->detectortype;
            analysisInfoVec.push_back(ai);
        }
        else {
            if (detectortype != ai->detectortype)
                throw runtime_error("Not all input files have same detector type.\n\n");
            analysisInfoVec.push_back(ai);
        }
        if (!f->GetListOfKeys()->Contains("tracks"))
            throw runtime_error("Not all input files have tracks TTrees.\n\n");
        f->Close();
        delete f;
    }
  
    DetectorGeometry* g = DetectorGeometryTools::GetDetectorGeometry(
                              analysisInfoVec.back()->detectortype);
    
    // Combine tracks TTree
    TChain* tracks = new TChain("tracks");
    for (auto fileName=inFileName.begin(); fileName!=inFileName.end(); fileName++) {
        tracks->Add((*fileName).c_str());
        cout << *fileName << " tracks TTree added to TChain.\n";
    }
    cout << '\n';
    cout << "Entries in tracks: " << tracks->GetEntries() << '\n'; 

    // tgc_analysis style
    SetAnalysisStyle();
    gErrorIgnoreLevel = kWarning

    // Initialize differential non linearity correction class
    // if (doDNLCorrection) 
    DNLCorrector dnlCorrector(dnlConfigFileName, g);

    // Deactivate all branches
    tracks->SetBranchStatus("*", 0);

    // Activate the branches you want to copy to output tree
    for (auto activeBranchName : {"eventnumber*", "trackX*", "trackYGaussian*", "trackYWeighted*", 
                                  "sumPDO*", "rms*", "sigma*", "pdo_strip*", "posCH*", 
                                  "relClPosGaussian*", "clSize*", "isSaturated*"}) {
        tracks->SetBranchStatus(activeBranchName, 1);
    }

    // Open output file
    // ROOT complains if you mix accessing a tree from one root file and writing to another
    // so opening the output file must happen here.

    // string outpath = argv[2];
    TFile* outFile = new TFile((outpath + tag + "reclustering.root").c_str(), "RECREATE");
    if (outFile->IsZombie())
        throw runtime_error("Error opening output file.\n\n");
    outFile->WriteObject(&inFileName, "inputFileNamesVector");
    // Writing analysis info object to file
    // Write entire vector of objects
    // This isn't ideal because you'll need a LinkDef to read it,
    // but I'm tired and have to move on.
    outFile->WriteObject(&analysisInfoVec, "analysisInfoVector"); 
    // Also write just one object from which to extract detectortype in strip_position_analysis
    outFile->WriteObject(ai, "analysisinfo");
    map<UShort_t, Double_t> amplitudes = dnlCorrector.GetAmplitudeMultiplicityMap();
    outFile->WriteObject(&amplitudes, "dnl_amplitude_map");

    cout << "Cloning CosmicsAnalysis tracks tree.\n";
    TTree* reclustered = tracks->CloneTree();
    cout << "Cloned!\n";
    // reclustered->SetName("reclustering_tracks");
    // pm->Add(reclustered, myTTree);
    cout << "Entries in reclustered: " << reclustered->GetEntries() << '\n';

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
    map <UShort_t, Double_t> newYRel;

    TBranch* ramplitudeBranch = reclustered->Branch("r_amplitude", &amplitude);
    TBranch* ramplitudeErrorBranch = reclustered->Branch("r_amplitudeError", &amplitudeError);
    TBranch* rmeanBranch = reclustered->Branch("r_mean", &mean);
    TBranch* rmeanErrorBranch = reclustered->Branch("r_meanError", &meanError);
    TBranch* rsigmaBranch = reclustered->Branch("r_sigma", &newSigma);
    TBranch* rsigmaErrorBranch = reclustered->Branch("r_sigmaError", &newSigmaError);
    TBranch* rndfBranch = reclustered->Branch("r_ndf", &ndf);
    TBranch* rchi2Branch = reclustered->Branch("r_chi2", &chi2);
    TBranch* ryrelBranch = reclustered->Branch("r_yrel", &newYRel);

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

    map<UShort_t, Double_t> yrel;
    map <UShort_t, Double_t>* yrelPtr;
    yrelPtr = &yrel;

    reclustered->SetBranchAddress("eventnumber", &eventnumber);
    reclustered->SetBranchAddress("trackYGaussian", &trackYGaussianPtr);
    reclustered->SetBranchAddress("trackYWeighted", &trackYWeightedPtr);
    reclustered->SetBranchAddress("sumPDO", &sumPDOPtr);
    reclustered->SetBranchAddress("rms", &rmsPtr);
    reclustered->SetBranchAddress("sigma", &sigmaPtr);
    reclustered->SetBranchAddress("pdo_strip", &pdoStripPtr);
    reclustered->SetBranchAddress("posCH", &posCHPtr);
    reclustered->SetBranchAddress("relClPosGaussian", &yrelPtr);

    UShort_t layer;
    vector<Double_t> pos;
    vector<Double_t> pdo;
    // yrel is relative position of hit with respect to hit strip's pitch
    Double_t cosmicsYRel; // from cosmics
    Double_t reclusteringYRel; // after refitting
    Double_t correctedYRel; // after DNL correction
    Double_t correctedMean; // after DNL correction
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
    // pm->Add("reclustering_mean", ";Cluster mean [mm];No.Clusters", yLims.second - yLims.first, 
    // yLims.first, yLims.second, myTH1F);
    pm->Add("reclustering_mean_error", ";Cluster mean error [mm];No.Clusters", 50, 0, 0.05, 
            myTH1F);
    pm->Add("reclustering_sigma",";#sigma [mm];No. Clusters", 100, 0, 10, myTH1F);
    pm->Add("cosmics_yrel", ";Cosmics y_{rel};No. Clusters", 20, -0.5, 0.5, myTH1F);
    pm->Add("reclustering_yrel", ";Reclustering y_{rel};No. Clusters", 20, -0.5, 0.5, myTH1F);
    pm->Add("corrected_yrel", ";DNL corrected y_{rel};No. Clusters", 20, -0.5, 0.5, myTH1F);
    pm->Add("cluster_mean_difference", ";#mu_{reclustering}-#mu_{cosmics};No. Entries", 30, -0.3,
            0.3, myTH1F);
    pm->Add("dnl_corrected_cluster_mean_difference", 
            ";#mu_{DNL corrected}-#mu_{cosmics};No. Entries", 30, -0.3, 0.3, myTH1F);
    // Spec multiplicity
    for (Int_t m=3; m<=8; m++) {
        pm->Add("cosmics_sigma_multiplicity_" + to_string(m), "Cluster size = " + to_string(m) + 
                ";Cosmics #sigma [mm];No. Clusters", 100, 0, 10, myTH1F);
        pm->Add("reclustering_amplitude_multiplicity_" + to_string(m), "Cluster size = " + 
                to_string(m) + ";Amplitude [ADC counts];No. Clusters", 320, 0, 3200, myTH1F);
        // pm->Add("reclustering_mean_multiplicity_" + to_string(m), "Cluster size = " + 
        //        to_string(m) + ";Cluster mean [mm];No.Clusters", yLims.second - yLims.first, 
        //        yLims.first, yLims.second, myTH1F);
        pm->Add("reclustering_mean_error_multiplicity_" + to_string(m), "Cluster size = " +
                to_string(m) + ";Cluster mean error [mm];No.Clusters", 20, 0, 0.05, myTH1F);
        pm->Add("reclustering_sigma_multiplicity_" + to_string(m), "Cluster size = " +
                to_string(m) + ";#sigma [mm];No. Clusters", 100, 0, 10, myTH1F);
        pm->Add("cosmics_yrel_multiplicity_" + to_string(m), "Cluster size = " + to_string(m) + 
                ";Cosmics y_{rel};No. Clusters", 20, -0.5, 0.5, myTH1F);
        pm->Add("reclustering_yrel_multiplicity_" + to_string(m), "Cluster size = " +
                to_string(m) + ";Reclustering y_{rel};No. Clusters", 20, -0.5, 0.5, myTH1F); 
        pm->Add("corrected_yrel_multiplicity_" + to_string(m), "Cluster size = " +
                to_string(m) + ";DNL corrected y_{rel};No. Clusters", 20, -0.5, 0.5, myTH1F); 
        pm->Add("dnl_corrected_cluster_mean_difference_multiplicity_" + to_string(m), 
                "Cluster size = " + to_string(m) + 
                ";#mu_{DNL corrected}-#mu_{cosmics};No. Entries", 30, -0.3, 0.3, myTH1F);
        if (m==3) {
            pm->Add("cluster_mean_difference_multiplicity_" + to_string(m), "Cluster size = " + 
                    to_string(m) + ";#mu_{reclustering}-#mu_{cosmics};No. Entries", 20, -0.01, 
                    0.01, myTH1F);
        }
        else {
            pm->Add("cluster_mean_difference_multiplicity_" + to_string(m), "Cluster size = " + 
                to_string(m) + ";#mu_{reclustering}-#mu_{cosmics};No. Entries", 30, -0.3, 0.3, 
                myTH1F);
        }
    }
    // Plots by layer
    for (UShort_t i=1; i<=4; i++) {
        pm->Add("reclustering_mean_layer_" + to_string(i), 
                "Layer " + to_string(i) + ";Cluster mean [mm];No.Clusters", 
                yLims.second - yLims.first, yLims.first, yLims.second, myTH1F);
        pm->Add("yrel_vs_y_layer_" + to_string(i), "Layer " + to_string(i) + ";y [mm];y_{rel}", 
                (yLims.second - yLims.first)/20, yLims.first, yLims.second, 20, -0.5, 0.5, myTH2F);
        pm->Add("yrel_vs_y_multiplicity_3_layer_" + to_string(i), 
                "Cluster size 3, layer " + to_string(i) + ";y [mm];y_{rel}", 
               (yLims.second - yLims.first)/40, yLims.first, yLims.second, 20, -0.5, 0.5, myTH2F);
    }
    // TH2F
    
    // Make list of plot names for printing to pdf
    vector<string> nameBases{"cosmics_sigma", "reclustering_amplitude",
                             "reclustering_mean_error", "reclustering_sigma", "cosmics_yrel", 
                             "reclustering_yrel", "corrected_yrel", "cluster_mean_difference",
                             "dnl_corrected_cluster_mean_difference"}; 
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
    // Layer-dependent TH1s
    for (UShort_t i=1; i<=4; i++) {
        plotNames.push_back("reclustering_mean_layer_" + to_string(i));
    }

    // File for output
    ofstream of;
    of.open(outpath + tag + "sample_cluster_fit.csv");
    cout << "Starting event loop...\n";
    // for (Int_t i=0; i<1; i++) {
    for (Int_t i=0; i<nEntries; i++) {
        // Get entry
        reclustered->GetEntry(i);
        // Clear the leaves to output
        amplitude.clear(); amplitudeError.clear();
        mean.clear(); meanError.clear();
        newSigma.clear(); newSigmaError.clear();
        ndf.clear(); chi2.clear();

        // A couple quads have excess noise on the long edge on certain layers
        // Use this scaffold to cut noise at high-y edge for certain quads
        // Known noisy quads include: 
        // QL2C04 3100V layer 2
        // QL2P01 3100V layer 1
        // QL2P02 3100V layer 2 (cut for y > 1169 mm)
        /*
        UShort_t noisyLayer = 2;
        if (trackYGaussian.find(noisyLayer) != trackYGaussian.end()) {
            if (trackYGaussian.at(noisyLayer) > 1173) {
                ramplitudeBranch->Fill();
                ramplitudeErrorBranch->Fill();
                rmeanBranch->Fill();
                rmeanErrorBranch->Fill();
                rsigmaBranch->Fill();
                rsigmaErrorBranch->Fill();
                rndfBranch->Fill();
                rchi2Branch->Fill();
                ryrelBranch->Fill();
                continue;
            }
        }*/

        
        // Assumes that posCH and pdoStrip are same size
        for (auto val=posCH.begin(); val!=posCH.end(); val++) {
            layer = val->first;
            pos = val->second;
            pdo = pdoStrip.at(layer);
            cosmicsYRel = yrel.at(layer);
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
            // cout << pos.size() << ' ' << pdo.size() << '\n';
            DoGausFitMinuit(pos, pdo, fitInfo, false);
            // If fitted mean is outside fiducial area, fail the fit
            if (!g->InFiducialArea(fitInfo.mean, GetSector("L"+to_string(layer)+"S"))) {
                fitInfo.fitResult=false; 
            }
            // If fit was successful, add fit parameters to branches of output tree
            // and do DNL correction if desired
            else if (fitInfo.fitResult!=0) { 
                // Calculate yrel of reclustering mean
                reclusteringYRel = dnlCorrector.CalculateYRel(fitInfo.mean, layer);
                if (doDNLCorrection) {
                    // correctedMean = dnlCorrector.ApplyCorrection(fitInfo.mean, layer);
                    correctedMean = dnlCorrector.ApplyCorrection(fitInfo.mean, layer, pos.size());
                    mean[layer] = correctedMean;
                    // Calculate yrel for the new, corrected mean
                    correctedYRel = dnlCorrector.CalculateYRel(correctedMean, layer);
                    newYRel[layer] = correctedYRel;
                    // cout << doDNLCorrection << ' ' << fitInfo.mean << ' ' << correctedMean << ' ' << cosmicsYRel << ' ' << newYRel[layer] << '\n';
                    if (!g->InFiducialArea(correctedMean, GetSector("L"+to_string(layer)+"S")))
                        fitInfo.fitResult=false;
                }
                else {
                    mean[layer] = fitInfo.mean;
                    newYRel[layer] = reclusteringYRel;
                    correctedYRel = reclusteringYRel;
                    // No difference in DNL corrected mean and reclustering mean
                    correctedMean = fitInfo.mean; 
                }
                // Put the rest of the fit parameters into their containers
                amplitude[layer] = fitInfo.A;
                amplitudeError[layer] = fitInfo.Aerr;
                meanError[layer] = fitInfo.meanErr;
                newSigma[layer] = fitInfo.sigma;
                newSigmaError[layer] = fitInfo.sigmaErr;
                ndf[layer] = fitInfo.NDF;
                chi2[layer] = fitInfo.chi2;
            }

            // Fill plots
            if  (fitInfo.fitResult!=0 && pos.size()>=3 && pos.size()<=8) { // If fit is good,
                pm->Fill("cosmics_sigma", sigma.at(layer));
                pm->Fill("reclustering_amplitude", fitInfo.A);
                // pm->Fill("reclustering_mean", fitInfo.mean);
                pm->Fill("reclustering_mean_error", fitInfo.meanErr);
                pm->Fill("reclustering_sigma", fitInfo.sigma);
                pm->Fill("cosmics_yrel", cosmicsYRel);
                pm->Fill("reclustering_yrel", reclusteringYRel);
                pm->Fill("corrected_yrel", correctedYRel);
                pm->Fill("cluster_mean_difference", fitInfo.mean - trackYGaussian.at(layer)); 
                pm->Fill("dnl_corrected_cluster_mean_difference", 
                         correctedMean - trackYGaussian.at(layer));
                multStr = to_string(pos.size());
                pm->Fill("cosmics_sigma_multiplicity_" + multStr, sigma.at(layer));
                pm->Fill("reclustering_amplitude_multiplicity_" + multStr, fitInfo.A);
                // pm->Fill("reclustering_mean_multiplicity_" + multStr, fitInfo.mean);
                pm->Fill("reclustering_mean_error_multiplicity_" + multStr, fitInfo.meanErr);
                pm->Fill("reclustering_sigma_multiplicity_" + multStr, fitInfo.sigma);
                pm->Fill("cosmics_yrel_multiplicity_" + multStr, cosmicsYRel);
                pm->Fill("reclustering_yrel_multiplicity_" + multStr, reclusteringYRel);
                pm->Fill("corrected_yrel_multiplicity_" + multStr, correctedYRel);
                pm->Fill("cluster_mean_difference_multiplicity_" + multStr, 
                        fitInfo.mean - trackYGaussian.at(layer));
                pm->Fill("dnl_corrected_cluster_mean_difference_multiplicity_" + multStr, 
                         correctedMean - trackYGaussian.at(layer));
                if (pos.size()==3) {
                    pm->Fill("yrel_vs_y_multiplicity_3_layer_" + to_string(layer), 
                             mean.at(layer), correctedYRel);
                }
                pm->Fill("reclustering_mean_layer_" + to_string(layer), mean.at(layer));
                pm->Fill("yrel_vs_y_layer_" + to_string(layer), mean.at(layer), correctedYRel);
            }
            // Output sample of fits to file
            if (i<50) {
                // event number, multiplicity, tgc_analysis mean, tgc_analysis sigma, fit result,
                // amplitude, amplitude error, mean, mean error, sigma, sigma error, chi2/ndf
                of << eventnumber  << ',' << layer << ',' << pos.size() << ',' << trackYGaussian.at(layer);
                of << ',' << sigma.at(layer) << ',' << fitInfo.fitResult << ',' << fitInfo.A;
                of << ',' << fitInfo.Aerr << ',' << fitInfo.mean << ',' << fitInfo.meanErr << ',';
                of << fitInfo.sigma << ',' << fitInfo.sigmaErr << ',' << fitInfo.chi2/fitInfo.NDF;
                of << '\n';
            }

            // May need to add a check here in case fit fails.
            if (fitInfo.fitResult==false) failedFitCount++; // Keep tally of # of fits that fail
            numFits++; 

        }
        // reclustered->Fill();
        ramplitudeBranch->Fill();
        ramplitudeErrorBranch->Fill();
        rmeanBranch->Fill();
        rmeanErrorBranch->Fill();
        rsigmaBranch->Fill();
        rsigmaErrorBranch->Fill();
        rndfBranch->Fill();
        rchi2Branch->Fill();
        ryrelBranch->Fill();
    } // End reclustering loop

    // reclustered->Write(); // Write tree to output file
    cout << "Notice: " << failedFitCount << " of " << numFits << " fits failed\n";
    // cout << "Notice: " << disagreesWithCosmicsCount << " means of " << numFits;
    // cout << " fits diagree with the cosmics mean\n";
    
    of.close();


    // Check for evidence of excess noise by fitting constant to distribution of reclustering mean.
    // and seeing if no. entries in each bin is 5 times higher than that constant
    for (UShort_t i=1; i<=4; i++) {
        TH1F* h = (TH1F*)pm->GetTH1F("reclustering_mean_layer_" + to_string(i));
        h->Fit("pol0", "W"); // Ignore bin errors when fitting constant
        TF1* f = h->GetFunction("pol0");
        Double_t C = f->GetParameter(0);
        for (Int_t b=1; b<h->GetNbinsX()+1; b++) {
            if (h->GetBinContent(b) > 5*C) {
                cout << "WARNING: there could be excessive noise on layer " << i << " near y = ";
                cout << h->GetBinCenter(b) << " mm. Check cluster mean distribution. \n";
            } 
        }
    }

    // Print plots
    TCanvas* c = new TCanvas();
    c->Print((outpath + tag + "reclustering_plots.pdf[").c_str());
    for (auto name=plotNames.begin(); name!=plotNames.end(); name++) {
        TH1F* h = (TH1F*)pm->GetTH1F(*name);
        h->Draw();
        c->Print((outpath + tag + "reclustering_plots.pdf").c_str());
        c->Clear();
    }
    // Now draw the TH2s
    for (UShort_t i=1; i<=4; i++) {
        TH2F* h = (TH2F*)pm->Get("yrel_vs_y_layer_" + to_string(i));
        h->Draw("colz");
        c->Print((outpath + tag + "reclustering_plots.pdf").c_str());
    }
    for (UShort_t i=1; i<=4; i++) {
        TH2F* h = (TH2F*)pm->Get("yrel_vs_y_multiplicity_3_layer_" + to_string(i));
        h->Draw("colz");
        c->Print((outpath + tag + "reclustering_plots.pdf").c_str());
    }
    c->Print((outpath + tag + "reclustering_plots.pdf]").c_str());

    // Write reclustering tree to output file
    // reclustered->Write(nullptr, TObject::kWriteDelete);
    // Write everything in plot manager to output file
    pm->Write(outFile);
    delete pm;
    reclustered->Write(nullptr, TObject::kWriteDelete);
    outFile->Close();
    delete outFile;
    delete tracks;
    return 0;
}

