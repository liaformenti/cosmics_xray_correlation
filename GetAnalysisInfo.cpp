// Function to get AnalysisInfo object from CosmicsAnalysis root file.
// Parts copied from tgc_analysis/ReadAnalysisInfo/ by Benoit Lefebvre
// As such, all conditions not tested fully, except success condition.
// Lia Formenti, April 27, 2020

#define GetAnalysisInfo_cxx
#include "GetAnalysisInfo.h"

using namespace std;

AnalysisInfo* GetAnalysisInfo(TFile* inROOT) {
    if(!inROOT->GetListOfKeys()->Contains("analysisinfo")) {
      throw runtime_error("Cannot find AnalysisInfo object in file.");
    }
  
    AnalysisInfo *info=nullptr;
  
    // Find the number of analysis info objects
    // There could be more than one if hAdd was used.
    Int_t pCounter=0;
    while(pCounter==0 || info!=nullptr){
      info = (AnalysisInfo*) gROOT->FindObject(("analysisinfo;"+to_string(pCounter+1)).c_str());
      pCounter++;
    }
    pCounter--;
    
    
    if(pCounter==0){
      // Should check if this should ever be true.
      cout<<"WARNING: No valid 'analysisinfo' object found in file"<<endl;
    }
    else if(pCounter>1){
      // Should check if I need a condition to deal with this. 
      cout<<"WARNING: Found "<<pCounter<<" 'analysisinfo' objects in file. Nothing was printed."<<endl;
      cout<<"Maybe you used 'hAdd' to combine 'CosmicsAnalysis' root files?"<<endl;
    }
    else{
      info = (AnalysisInfo*) gROOT->FindObject(("analysisinfo;"+to_string(pCounter)).c_str());
      // info->PrintContent();
    }
   
    return info;
}
