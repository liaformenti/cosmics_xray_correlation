#define Tools_cxx
#include "Tools.h"

namespace Tools{

  std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, std::back_inserter(elems));
    return elems;
  }


  Bool_t IsNumber(const std::string &s){
    return std::find_if(s.begin(), s.end(), [](char c) { return !std::isdigit(c); } ) == s.end();
  }

  
  Int_t CInt(const std::string str){
    return atoi(str.c_str());
  }

  std::string CStr(const Int_t i){
    return std::to_string(i);
  }

  std::string CStr(const UInt_t i){
    return CStr((Int_t) i);
  }
  
  std::string CStr(const Double_t f){
    return std::to_string(f);
  }

  std::string CStr(const Bool_t b){
    return (b ?  "true" : "false");
  }

  std::string CStr(const Double_t f, const Int_t precision){
    char buffer[256];
    sprintf(buffer, "%.2f", f);
    return std::string(buffer);
  }
  
  std::string CStr(const Sector &s){
    return Print(s);
  }

  std::string CStr(const ElectrodeType t){
    switch(t){
    case fSTRIP: return "strip";
    case fWIRE:  return "wire";
    case fPAD:   return "pad";
    default: throw std::runtime_error("Electrode type not recognized.");
    }
  }

  Double_t DegToRad(const Double_t angleDeg){
    return angleDeg*TMath::DegToRad();
  }

 

  // Time stamp in the format:
  // yymmddhhmmss
  std::string GetTimeStamp(){
    time_t t = time(0);   // get time now
    struct tm *now = localtime( & t );
    char buffer[256];
    sprintf(buffer, "%04i%02i%02i%02i%02i%02i", now->tm_year+1900,
	   now->tm_mon, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec);

    return std::string(buffer);
  }


  // Find the number of layers of detector
  // DO NOT KNOW WHY IT IS NECESSARY
  //  UShort_t GetNLayers(const AnalysisInfo &info){  
  //    DetectorGeometry *g = Tools::GetDetectorGeometry(info.detectorname);
  //    UShort_t n = g->GetLayers().size();
  //    delete g;
  //    return n;
  //  }
  

  std::vector<Sector> SetToVector(std::set<Sector> s){
    std::vector<Sector> v;
    std::copy(s.begin(), s.end(), std::back_inserter(v));
    return v;
  }

  Bool_t fExists(const std::string &name) {
    struct stat buffer;
    return (stat (name.c_str(), &buffer) == 0);
  }
  
}


