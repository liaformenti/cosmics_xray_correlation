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

// For fitting clusters
// Modified from tgc_analysis Clustering::DoGausFitMinuit by Benoit
// WARNING: Fit range is HARD CODED for clusters!! == data limits +/- 1.6
// Gaussian fit using the ROOT minimizer Minuit2
// The ROOT minimization is not thread safe!
void DoGausFitMinuit(const vector<Double_t> &x, const vector<Double_t> &y,
				 GausFitInfo &info, Bool_t verbose){
  if(verbose) cout<<"[Cluster::DoGausFitMinuit]"<<endl;
  if(x.size()!=y.size()) throw logic_error("Invalid parameters.");
  if(x.size()<3) throw logic_error("Cannot process gaussian fit with points.");
  if(verbose){
    cout<<"x\ty"<<endl;
    for(int i=0; i<x.size(); i++) cout<<x.at(i)<<"\t"<<y.at(i)<<endl;
  }
    
  TF1 *fGaus = new TF1("fGaus", "gaus", x.front()-1.6, x.back()+1.6); // You may have to change
  fGaus->SetParameter(0, info.A);
  fGaus->SetParameter(1, info.mean);
  fGaus->SetParameter(2, info.sigma);

  
  // TGraph *gGaus = new TGraph(x.size(), &(x[0]), &(y[0]));
  TH1F *hGaus = new TH1F("", "", x.size(), x.front()-1.6, x.back()+1.6);
  for(int i=0; i<x.size(); i++){
    hGaus->SetBinContent(i+1, y[i]);
  }

  string opt;
  if(!verbose)
    opt="Q";
  
  int fitResult = hGaus->Fit("fGaus", (opt+"RW").c_str());
  if(fitResult==0){
    info.fitResult=true; // Indicates fit succeeded
    info.A = fGaus->GetParameter(0);
    info.Aerr = fGaus->GetParError(0);
    info.mean = fGaus->GetParameter(1);
    info.meanErr = fGaus->GetParError(1);
    info.sigma = fGaus->GetParameter(2);
    info.sigmaErr = fGaus->GetParError(2);
    info.NDF = x.size()-3;
    info.chi2 = hGaus->GetFunction("fGaus")->GetChisquare();

    if(verbose){
      cout<<"chi2 of fit="<<info.chi2<<endl;
    }
  }
  else{
    info.fitResult = false; // Indicates fit failed
    info.A = numeric_limits<Double_t>::quiet_NaN();
    info.mean = numeric_limits<Double_t>::quiet_NaN();
    info.sigma = numeric_limits<Double_t>::quiet_NaN();
    info.NDF = -1;
    info.chi2 = numeric_limits<Double_t>::quiet_NaN(); 
  }

  delete hGaus;
  //  delete gGaus;  
  delete fGaus;

}
// Taken from tgc_analysis Clustering.cpp
// Gaussian fit using the Guos matrix technique
// This function is thread-safe and fast but has accuracy limitations
// Source 1: https://www.degruyter.com/downloadpdf/j/jee.2015.66.issue-3/jee-2015-0029/j
//           ee-2015-0029.pdf
// Source 2: https://www.researchgate.net/publication/252062037_A_Simple_Algorithm_for_F
//           itting_a_Gaussian_Function_DSP_Tips_and_Tricks 
void DoGausFitGuos(const vector<Double_t> &x, const vector<Double_t> &y,
			       GausFitInfo &info, Bool_t verbose){
  if(verbose) cout<<"[Cluster::DoGausFitGuos]"<<endl;
  if(x.size()!=y.size()) throw logic_error("Invalid parameters.");
  if(x.size()<3) throw logic_error("Cannot process gaussian fit with points.");
  
  Int_t N = x.size();
  Double_t S=0, Sx=0, Sxx=0, Sxxx=0, Sxxxx=0, Slny=0, Sxlny=0, Sxxlny=0;
  for(int i=0; i<N; i++){
    Double_t xPow=x[i];
    Double_t yPow = y[i]*y[i];
  
    S     += yPow;
    Sx    += xPow*yPow;  xPow*=x[i];
    Sxx   += xPow*yPow;  xPow*=x[i];
    Sxxx  += xPow*yPow;  xPow*=x[i];
    Sxxxx += xPow*yPow;
    
    Double_t lnyPow = log(y[i]);
    Slny   += lnyPow*yPow; lnyPow*=x[i];
    Sxlny  += lnyPow*yPow; lnyPow*=x[i];
    Sxxlny += lnyPow*yPow;
  }
  
  Double_t matData[9] = {S, Sx, Sxx, Sx, Sxx, Sxxx, Sxx, Sxxx, Sxxxx};
  Double_t vecData[3] = {Slny, Sxlny, Sxxlny};

  if(verbose){
    cout<<"Sx="<<Sx<<endl;
    cout<<"Sxx="<<Sxx<<endl;
    cout<<"Sxxx="<<Sxxx<<endl;
    cout<<"Sxxxx="<<Sxxxx<<endl;
    cout<<"Slny="<<Slny<<endl;
    cout<<"Sxlny="<<Sxlny<<endl;
    cout<<"Sxxlny="<<Sxxlny<<endl;
  }
  
  TMatrixD mat(3, 3, matData);
  TVectorD vec(3, vecData);
  mat.Invert();
  vec *= mat;

  Double_t a=vec[0], b=vec[1], c=vec[2];

  if(verbose){
    cout<<"a="<<a<<endl;
    cout<<"b="<<b<<endl;
    cout<<"c="<<c<<endl;
  }
    
  Double_t A = TMath::Exp(a-b*b/(4*c));
  Double_t mean = -b/(2*c);
  Double_t sigma = sqrt(-1./(2*c));

  if(!::isnan(A) && !::isnan(mean) && !::isnan(sigma)){
    info.fitResult=true;
    info.A = A;
    info.mean = mean;
    info.sigma = sigma;
    info.NDF = x.size()-3;
    
    info.chi2=0;
    for(int i=0; i<N; i++){
      Double_t valGaus = A*TMath::Gaus(x[i], mean, sigma);
      Double_t res = valGaus - y[i];
      info.chi2 += pow(res, 2);
    }
  }
  else{
    info.fitResult=false;
    info.A = numeric_limits<Double_t>::quiet_NaN();
    info.mean = numeric_limits<Double_t>::quiet_NaN();
    info.sigma = numeric_limits<Double_t>::quiet_NaN();
    info.NDF = -1;
    info.chi2 = numeric_limits<Double_t>::quiet_NaN();
  }

  if(verbose){
    cout<<"*****************"<<endl;
    cout<<"GUOS GAUSSIAN FIT"<<endl;
    info.Print();
  }
    
}

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

Double_t CalculateYRelFromResidual(UShort_t layer, Double_t residual, Double_t trackY, 
                                   DetectorGeometry* g) {
    Double_t yHit = residual + trackY;
    // cout << "Position: " << yHit << ' ' << "layer: " << layer << '\n';
    UShort_t stripID = g->GetID(yHit, GetSector("L"+to_string(layer)+"S"));
    // cout << "StripID: " << stripID << '\n'; 
    Channel stripCH = {fSTRIP, layer, stripID};
    // cout << "Channel: " << stripCH.CStr(" ") << '\n';
    Double_t stripCenter = g->PositionChannel(stripCH);
    // cout << "Strip center: " << stripCenter << '\n';
    Double_t yrel = (yHit - stripCenter)/g->GetPitch(fSTRIP);
    // cout << "yrel: " << yrel << '\n';
    return yrel;
}


