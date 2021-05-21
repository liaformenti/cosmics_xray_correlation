/* *******************************
 * Class for setting differential non linearity parameters, including method to apply correction.
 * Uses DNL configuration file.
 * Format: multiplicity amplitude [mm]
 * Multiplicity of zero is a special key for a universal amplitude parameter.
 * The universal amplitude parameter is used for clusters with multiplicities not listed in
 * the amplitudeMultiplicity map.
 * Zero (no correction) is the default universal amplitude parameter.
 * */
// C++ includes

#ifndef DNL_h
#define DNL_h

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <map>

// ROOT includes
#include <TMath.h>

// tgc_analysis includes
#include "DetectorGeometry.h"

class DNLCorrector {
  public: 
      DNLCorrector(){};
      DNLCorrector(std::string nameOfConfigFile, DetectorGeometry* _g);
      ~DNLCorrector(){};

      // Members
      std::string configFileName;
      // Methods
      Double_t GetUniversalAmplitude();
      std::map<UShort_t, Double_t> GetAmplitudeMultiplicityMap();
      Double_t ApplyCorrection(Double_t y, UShort_t layer);
      // Apply correction when amplitude varies with multiplicity
      Double_t ApplyCorrection(Double_t y, UShort_t layer, Int_t clSize);
      Double_t CalculateYRel(Double_t val, UShort_t layer);

   private:
      // Members
      DetectorGeometry* g = nullptr;
      Double_t universalAmplitude = 0;
      // Map for if you want amplitude by multiplicity
      // Write this to ouput file for auto documentation
      std::map<UShort_t, Double_t> amplitudeMultiplicityMap; 
      // Methods
       void ParseDNLConfig();
};

#endif
