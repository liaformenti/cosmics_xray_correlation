#define DetectorGeometryTools_cxx
#include "DetectorGeometryTools.h"

using namespace std;

set<string> DetectorGeometryTools::typeList;

DetectorGeometry *DetectorGeometryTools::
GetDetectorGeometry(const char *detectortype){
  return GetDetectorGeometry(string(detectortype));
}

DetectorGeometry *DetectorGeometryTools::
GetDetectorGeometry(TString detectortype){
  return GetDetectorGeometry(string(detectortype.Data()));
}

DetectorGeometry *DetectorGeometryTools::
GetDetectorGeometry(string detectortype){
  //  DetectorGeometry *geometry = nullptr;

  //  if(!IsDetectorType(detectortype)){
  //    const string message("Detector type not supported by ChannelGeometry.");
  //    throw runtime_error(message);
  //  }

  auto nswTypes = DetectorGeometryNSW::GetSupportedTypes();

  if(nswTypes.find(detectortype)!=nswTypes.end()){
    return new DetectorGeometryNSW(detectortype);
  }
  else{
    const string message("Detector type not supported by ChannelGeometry.");
    throw runtime_error(message);
  }
}

void DetectorGeometryTools::MakeDetectorTypesList(){
  auto nswTypes = DetectorGeometryNSW::GetSupportedTypes();
  typeList.insert(nswTypes.begin(), nswTypes.end());
}

Bool_t DetectorGeometryTools::IsDetectorType(string detectortype){
  MakeDetectorTypesList();
  return typeList.find(detectortype) != typeList.end();
}

void DetectorGeometryTools::PrintDetectorTypes(){
  MakeDetectorTypesList();
  for(const auto t : typeList){ cout<<t<<" "; }
  cout<<endl;
}


  

