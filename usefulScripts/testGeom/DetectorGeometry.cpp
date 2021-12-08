#define DetectorGeometry_cxx
#include "DetectorGeometry.h"

std::runtime_error DetectorGeometry::
invalidParameters = std::runtime_error("Invalid parameters.");

Double_t DetectorGeometry::GetPitch(ElectrodeType type){
  if(type==fSTRIP) return stripPitch;
  else if(type==fWIRE) return wirePitch;
  else throw invalidParameters;
}
