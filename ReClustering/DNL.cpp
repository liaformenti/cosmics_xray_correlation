#define DNL_cxx
#include "DNL.h"

using namespace std;

DNLCorrector::DNLCorrector(string nameOfConfigFile, DetectorGeometry* _g) : g(_g) {
    configFileName = nameOfConfigFile;
    ParseDNLConfig();
    // amplitudeMultiplicityMap.insert(pair<UShort_t, Double_t>(0, universalAmplitude));
}

void DNLCorrector::ParseDNLConfig() {
    // If no config file name, no correction to be applied, so just return
    // Universal amplitude is set to zero by default.
    if (configFileName=="") {
        cout << "Info: no DNL configuration file provided. Default is no DNL correction.\n\n";
        return;
    }
    ifstream f(configFileName);
    if (!f.is_open()) throw runtime_error("Error opening DNL configuration file.\n\n");
    cout << "Parsing DNL configuration file...\n";
    cout << "Multiplicity, DNL correction amplitude:\n";
    string line;
    Int_t splitIndex = 0;
    string token;
    Int_t mult;
    Double_t amp;
    while (getline(f, line)) {
        // Parse out mutlipliticy and amplitude
        splitIndex = line.find(' ');
        // Catches newline at end of file
        if (line == "") continue; 
        // Catches weird non-spaced input.
        else if (splitIndex<=0 || splitIndex>=(line.size()-1))
            throw runtime_error("Error parsing DNL configuration file: bad spacing.\n\n");
        //cout << splitIndex << '\n';
        token = line.substr(0, splitIndex);
        // cout << token << '\n';
        // Should be the multiplicity
        if (sscanf(token.c_str(), "%d", &mult) == 1) {
            // cout << mult << '\n';
        }
        else throw runtime_error("Error parsing DNL configuration file: bad multiplicity.\n\n");
        token = line.substr(splitIndex+1, line.size());
        // cout << token << '\n';
        if (sscanf(token.c_str(), "%lf", &amp) == 1) {
            // cout << amp << '\n';
        }
        else throw runtime_error("Error parsing DNL configuration file: bad amplitude.\n\n");
        // Note: ignores text after the mult/amp if not separately by a space.
        
        // Choose what to do with it
        // Universal amplitude case
        if (mult==0) {
            // If universal amplitude is uninitialized,
            if (universalAmplitude==0) universalAmplitude = amp;
            else throw runtime_error("Error parsing DNL configuration file: more than one universal amplitude parameter.\n\n");
        }
        else if ( (mult < 0) || (mult > 25) ) // If mult is out of range
            throw runtime_error("Error parsing DNL configuration file: invalid multiplicity.\n\n");
        else {  // valid multiplicity, check if a key already exists.
            if (amplitudeMultiplicityMap.find(mult) != amplitudeMultiplicityMap.end())
               throw runtime_error("Error parsing DNL configuration file: duplicate amplitudes for given multiplicity.\n\n"); 
        }
        cout << mult << ' ' << amp << '\n';
        amplitudeMultiplicityMap.insert(pair<UShort_t, Double_t>(mult, amp)); 
        // Add multiplicity and amplitude to map
    }
    // cout << universalAmplitude << '\n';
    f.close();

    return;
}

Double_t DNLCorrector::GetUniversalAmplitude() { return universalAmplitude; }

map<UShort_t, Double_t> DNLCorrector::GetAmplitudeMultiplicityMap() { return amplitudeMultiplicityMap; }

Double_t DNLCorrector::ApplyCorrection(Double_t y, UShort_t layer) {
    Double_t yrel = CalculateYRel(y, layer);
    return y + universalAmplitude*TMath::Sin(2*TMath::Pi()*yrel);
}

// Apply correction when it depends on multiplicity
Double_t DNLCorrector::ApplyCorrection(Double_t y, UShort_t layer, Int_t clSize) {
    Double_t yrel = CalculateYRel(y, layer);
    Double_t yCorr = y;
    if (amplitudeMultiplicityMap.find(clSize) != amplitudeMultiplicityMap.end()) {
        yCorr += amplitudeMultiplicityMap.at(clSize)*TMath::Sin(2*TMath::Pi()*yrel);
        // cout << clSize << ' ' << amplitudeMultiplicityMap.at(clSize) << ' ' << yrel << ' '<< y << ' ' << yCorr << '\n';
    }
    else {
        yCorr += universalAmplitude*TMath::Sin(2*TMath::Pi()*yrel);
        // cout << clSize << ' ' << universalAmplitude << ' ' << yrel << ' ' << y << ' ' << yCorr << '\n';
    }
    return yCorr;
}

Double_t DNLCorrector::CalculateYRel(Double_t val, UShort_t layer) {
    // cout << "Position: " << val << ' ' << "layer: " << layer << '\n';
    UShort_t stripID = g->GetID(val, GetSector("L"+to_string(layer)+"S"));
    // cout << "StripID: " << stripID << '\n'; 
    Channel stripCH = {fSTRIP, layer, stripID};
    // cout << "Channel: " << stripCH.CStr(" ") << '\n';
    Double_t stripCenter = g->PositionChannel(stripCH);
    // cout << "Strip center: " << stripCenter << '\n';
    Double_t yrel = (val - stripCenter)/g->GetPitch(fSTRIP);
    // cout << "yrel: " << yrel << '\n';
    return yrel;
}
