// Function to get AnalysisInfo object from CosmicsAnalysis root file.
// Parts copied from tgc_analysis/ReadAnalysisInfo/ by Benoit Lefebvre
// Lia Formenti, April 27, 2020

#define GetAnalysisInfo_cxx
#include "GetAnalysisInfo.h"

using namespace std;

AnalysisInfo* GetAnalysisInfo(TFile* inROOT) {
    // AnalysisInfo* info = nullptr;
    cout << "called" << endl;
    // info = (AnalysisInfo*) in->Get("analysisinfo");
    // Get RunInfo object
    if(!inROOT->GetListOfKeys()->Contains("analysisinfo")) {
      // Gotta figure out this throw thing, else return a null ptr here
      // throw runtime_error("Cannot find AnalysisInfo object in file.");
      cout << "Cannot find AnalysisInfo object in file." << endl;
      return nullptr;
    }
  
    AnalysisInfo *info=nullptr;
    //  inROOT->GetObject("analysisinfo", info);
  
    // Find the number of analysis info objects
    // There could be more than one if hAdd was used.
    Int_t pCounter=0;
    while(pCounter==0 || info!=nullptr){
      info = (AnalysisInfo*) gROOT->FindObject(("analysisinfo;"+to_string(pCounter+1)).c_str());
      pCounter++;
    }
    pCounter--;
    
    
    if(pCounter==0){
      cout<<"WARNING: No valid 'analysisinfo' object found in file"<<endl;
    }
    else if(pCounter>1){
      cout<<"WARNING: Found "<<pCounter<<" 'analysisinfo' objects in file. Nothing was printed."<<endl;
      cout<<"Maybe you used 'hAdd' to combine 'CosmicsAnalysis' root files?"<<endl;
    }
    else{
      info = (AnalysisInfo*) gROOT->FindObject(("analysisinfo;"+to_string(pCounter)).c_str());
      cout << "gotcha" << endl;
      cout << info->detectortype << endl;
      // info->PrintContent();
    }
   
    return info;
}
