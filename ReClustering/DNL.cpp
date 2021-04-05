#define DNL_cxx
#include "DNL.h"

using namespace std;

DNLCorrector::DNLCorrector(string nameOfConfigFile, DetectorGeometry* _g) : g(_g) {
    configFileName = nameOfConfigFile;
    ParseDNLConfig();
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

        // Choose what to do with it
        // Universal amplitude case
        if (mult==0) {
            // If universal amplitude is uninitialized,
            if (universalAmplitude==0) universalAmplitude = amp;
            else throw runtime_error("Error parsing DNL configuration file: more than one universal amplitude parameter.\n\n");
        }
        if (mult != 0) {
            throw runtime_error("Error parsing DNL configuration file: amplitudes by multiplicity have not been coded in yet.\n\n");
        }
    }
    // cout << universalAmplitude << '\n';
    f.close();

    return;
}

Double_t DNLCorrector::GetUniversalAmplitude() { return universalAmplitude; }

Double_t DNLCorrector::ApplyCorrection(Double_t y, UShort_t layer) {
    Double_t yrel = CalculateYRel(y, layer);
    return y + universalAmplitude*TMath::Sin(2*TMath::Pi()*yrel);
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
