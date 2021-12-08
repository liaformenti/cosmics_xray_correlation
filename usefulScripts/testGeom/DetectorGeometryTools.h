#ifndef DetectorGeometryTools_h
#define DetectorGeometryTools_h

// C++ includes
#include <string>
#include <set>
#include <iostream> 

// ROOT includes
#include <TString.h>

// My Includes
#include "DetectorGeometry.h"
#include "DetectorGeometryNSW.h"

class DetectorGeometryTools{
 public:
  DetectorGeometryTools() = delete;
  ~DetectorGeometryTools(){};

  static DetectorGeometry *GetDetectorGeometry(const char *detectortype);
  static DetectorGeometry *GetDetectorGeometry(TString detectortype);
  static DetectorGeometry *GetDetectorGeometry(std::string detectortype);
  static Bool_t IsDetectorType(std::string detectortype);
  static void PrintDetectorTypes();

 private:
  static void MakeDetectorTypesList();
  static std::set<std::string> typeList;
};

#endif
