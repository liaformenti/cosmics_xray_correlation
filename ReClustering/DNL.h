/* *******************************
 * Class for setting differential non linearity parameters, including method to apply correction.
 * For now, just going to code and DNL parser and corrector that handles a universal amplitude
 * input only. But, to do by multiplicity, keep universal parameter, fill amplitudeMultiplicityMap
 * with amplitudes for each desired multiplicity. If there's an entry in the map for the
 * multiplicity, use that parameter, else use the universal one.
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
      // Double_t CalculateYRel(Double_t val, UShort_t layer);
      // Double_t ApplyCorrection(Double_t val);

   private:
      // Members
      DetectorGeometry* g = nullptr;
      Double_t universalAmplitude = 0;
      // std::map<UShort_t, Double_t> amplitudeMultiplicityMap; 
      // Methods
      void ParseDNLConfig();
};

#endif
