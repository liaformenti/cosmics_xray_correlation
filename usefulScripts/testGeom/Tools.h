//********************************************************************
// Set of generic tools not specific to any class.
//
// Author: Benoit Lefebvre               Date:  2017-07-05
//         lefebben@physics.mcgill.ca
//********************************************************************
#ifndef Tools_h
#define Tools_h

// C++ includes
#include <string>
#include <sstream>
#include <vector>
#include <iterator>
#include <stdexcept>
#include <ctime>
#include <set>
#include <sys/stat.h>
#include <unistd.h>

// ROOT includes
#include <Rtypes.h>
#include <TFile.h>
#include <TTree.h>
#include <TMath.h>

// My includes
#include "Sector.h"

namespace Tools{

  template <typename M>
    void PrintMap(const M &myMap){
    if(myMap.empty()){
      std::cout<<"\t(EMPTY)"<<std::endl;
      return;
    }
    
    for(auto it=myMap.begin(); it!=myMap.end(); it++){
      std::cout<<"\t"<<it->first<<" "<<it->second<<std::endl;
    }
  }
  
    
  template<typename Out>
    void split(const std::string &s, char delim, Out result) {
    std::stringstream ss;
    ss.str(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
      *(result++) = item;
    }
  }
  
  
  std::vector<std::string> split(const std::string &s, char delim);

  // "Convert to" functions
  Bool_t IsNumber(const std::string &s);
  Int_t CInt(const std::string str);
  std::string CStr(const Int_t i);
  std::string CStr(const UInt_t i);
  std::string CStr(const Double_t f);
  std::string CStr(const Bool_t b);
  std::string CStr(const Double_t f, const Int_t precision);
  std::string CStr(const Sector &s);
  std::string CStr(const ElectrodeType t);

  Double_t DegToRad(const Double_t angleDeg);
  

  inline Sector GetSector(const Channel &ch){
    return {ch.electrode_type, ch.layer};
  }
  
  // Find if vector had duplicates
  /*
  template <typename T>
    bool HasDuplicates(const std::vector<T>& vec){
    std::set<T> test;
    for (typename std::vector<T>::const_iterator it = vec.begin();
	 it != vec.end(); ++it) {
      if (!test.insert(*it).second) {
	return false;
      }
    }
    return true;
  }
  */

  // Get a string in the format yyyymmddhhss
  std::string GetTimeStamp();

  //  UShort_t GetNLayers(const AnalysisInfo &info);

  std::vector<Sector> SetToVector(std::set<Sector> s);
  Bool_t fExists(const std::string& name);

  
}

#endif
