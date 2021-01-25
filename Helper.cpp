#define Helper_cxx
#include "Helper.h"

using namespace std;

string Combination::String() const {
    string s = "layer" + to_string(layer) + "_fixedlayers" + to_string(fixed1) + to_string(fixed2);
    return s;
}

void Combination::Print() {
    cout << "Layer: " << layer << ", Fixed Layers: " << fixed1 << fixed2 << '\n';
    return;
}

bool Combination::operator==(const Combination& c) const {
    return ( (layer == c.layer) && (fixed1 == c.fixed1) && (fixed2 == c.fixed2) );
}

bool Combination::operator!=(const Combination& c) const {
    return ( (layer != c.layer) || (fixed1 != c.fixed1) || (fixed2 != c.fixed2) );
}



vector<Combination> combinationVector() {
    vector<Combination> vComb;
    vComb.push_back(Combination(3, 1, 2));
    vComb.push_back(Combination(4, 1, 2));
    vComb.push_back(Combination(2, 1, 3));
    vComb.push_back(Combination(4, 1, 3));
    vComb.push_back(Combination(2, 1, 4));
    vComb.push_back(Combination(3, 1, 4));
    vComb.push_back(Combination(1, 2, 3));
    vComb.push_back(Combination(4, 2, 3));   
    vComb.push_back(Combination(1, 2, 4));
    vComb.push_back(Combination(3, 2, 4));
    vComb.push_back(Combination(1, 3, 4));
    vComb.push_back(Combination(2, 3, 4));
    return vComb;
}

// Input info constructor
// Based on quadname, fills wedge id with the ID string of the wedge
// that quad is a part of
InputInfo::InputInfo(string theQuadName, string theDatabase, string theOutPath, string theTag) : 
quadname(theQuadName), database(theDatabase), outpath(theOutPath), tag(theTag) {};

void getOtherLayers(UShort_t la, UShort_t lb, UShort_t* lc, UShort_t* ld) {
    string throwStatement = "Invalid layer numbers: " + to_string(la);
    throwStatement += ", " + to_string(lb) + ", (getOtherLayers, ";
    throwStatement += "Helper.cpp).\n\n";
    switch(la) {
        case 1 : switch(lb) {
                     case 2 : *lc=3; *ld=4;
                              return;                        
                     case 3 : *lc=2; *ld=4;
                              return;
                     case 4 : *lc=2; *ld=3;
                              return;
                     default : throw invalid_argument(throwStatement);
                  }
        case 2 : switch(lb) {
                     case 3 : *lc=1; *ld=4;
                              return;
                     case 4 : *lc=1; *ld=3;
                              return;
                     default : throw invalid_argument(throwStatement);

                 }
        case 3 : switch(lb) {
                     case 4 : *lc=1; *ld=2;
                              return;
                     default : throw invalid_argument(throwStatement);
                 }
        default : throw invalid_argument(throwStatement);
    }
    return;
}
// string getCombinationPlotName(string header, string footer, UShort_t layer, UShort_t

// From tgc_analysis FinalizeAnalysis.cpp
// Modified for this purpose
// Used for fitting residual histograms

// DoubGausFitResult FinalizeAnalysis::
// DoDoubGausFit(const string hName){
DoubGausFitResult DoDoubGausFit(const string hName, PlotManager* pm) {
//  if(isVerbose){
//    cout<<"[FinalizeAnalysis::DoDoubGausFit]"<<endl;
//    cout<<"Fitting histogram '"<<hName<<"'"<<endl;
//  }
//  

  DoubGausFitResult res;
  res.fitResult = fFAILED;
  
  // Check if hist exists
  if(!pm->objExist(hName)){
    cerr<<"WARNING: Histogram '"<<hName<<"' not found in func. [DoDoubGausFit]"<<endl;
    return res;
  }

  // TH1F *h = pm->GetTH1F(hName);
  TH1I *h = pm->GetTH1I(hName);

  // Check if enough stats
//  if(h->GetEntries()<analysisInfo->minStatHisto){
//    cerr<<"WARNING: Not enough entries to fit histogram '"<<hName
//	<<"' in [DoDoubGausFit]."<<endl;
//    return res;
//  }
//  
  string fName = "fGaus_"+hName;
  string fVal = "[1]*TMath::Exp(-0.5*((x-[0])/[2])**2)+";
  fVal       += "[3]*TMath::Exp(-0.5*((x-[0])/[4])**2)";
  Double_t min = h->GetBinLowEdge(1);
  Double_t max = h->GetBinLowEdge(h->GetXaxis()->GetNbins()+1);

  TF1 *fGaus = new TF1(fName.c_str(), fVal.c_str(), min, max);
  fGaus->SetParName(0, "#mu");
  fGaus->SetParName(1, "A_{s}");
  fGaus->SetParName(2, "#sigma_{s}");
  fGaus->SetParName(3, "A_{b}");
  fGaus->SetParName(4, "#sigma_{b}");
  
  Double_t meanEst = h->GetMean();
  Double_t rmsEst = h->GetRMS();
  Double_t estAmp = 0.4*h->GetEntries()/rmsEst;
  fGaus->SetParameter(0, meanEst);
  fGaus->SetParameter(1, estAmp);
  fGaus->SetParameter(2, rmsEst);
  fGaus->SetParameter(3, estAmp);
  fGaus->SetParameter(4, rmsEst);


  TFitResultPtr r = h->Fit(fName.c_str(), "RSM");
  if(r!=0){
    res.fitResult = fFAILED; 
    return res;
  }

  //  Int_t fitStatus = r;
  string statusStr(gMinuit->fCstatu.Data());
  if(statusStr.find("CONVERGED")!=string::npos)
    res.fitResult = fSUCCESS;
  else
    res.fitResult = fFAILED;

  if(res.fitResult != fSUCCESS)
    return res;

  fGaus = h->GetFunction(fName.c_str());
  
  // sigma and amplitude must be positive
  fGaus->SetParameter(2, fabs(fGaus->GetParameter(2)));
  fGaus->SetParameter(4, fabs(fGaus->GetParameter(4)));

  // background sigma must be larger than signal
  if(fGaus->GetParameter(4)<fGaus->GetParameter(2)){
    Double_t tmpSigma = fGaus->GetParameter(4);
    fGaus->SetParameter(4, fGaus->GetParameter(2));
    fGaus->SetParameter(2, tmpSigma);

    Double_t tmpSigmaError = fGaus->GetParError(4);
    fGaus->SetParError(4, fGaus->GetParError(2));
    fGaus->SetParError(2, tmpSigmaError);

    Double_t tmpAmp = fGaus->GetParameter(1);
    fGaus->SetParameter(1, fGaus->GetParameter(3));
    fGaus->SetParameter(3, tmpAmp);

    Double_t tmpAmpError = fGaus->GetParError(1);
    fGaus->SetParError(1, fGaus->GetParError(3));
    fGaus->SetParError(3, tmpAmpError);
  }

  // Fill DoubGausFitResult struct
  res.mean=fGaus->GetParameter(0);
  res.meanError=fGaus->GetParError(0);
  
  res.amplitudeSignal=fGaus->GetParameter(1);
  res.amplitudeSignalError=fGaus->GetParError(1);
  res.sigmaSignal=fGaus->GetParameter(2);
  res.sigmaSignalError=fGaus->GetParError(2);

  res.amplitudeBackground=fGaus->GetParameter(3);
  res.amplitudeBackgroundError=fGaus->GetParError(3);
  res.sigmaBackground=fGaus->GetParameter(4);
  res.sigmaBackgroundError=fGaus->GetParError(4);

  return res;
}
