#define DNL_cxx
#include "DNL.h"

using namespace std;

DNLCorrector::DNLCorrector(string nameOfConfigFile, DetectorGeometry* _g) : g(_g) {
    configFileName = nameOfConfigFile;
    ParseDNLConfig();
}

void DNLCorrector::ParseDNLConfig() {
    ifstream f(configFileName);
    if (!f.is_open()) throw runtime_error("Error opening DNL configuration file.\n\n");
    string line;
    Int_t splitIndex = 0;
    string token;
    Int_t mult;
    Double_t amp;
    while (getline(f, line)) {
        splitIndex = line.find(' ');
        // Catches newline at end of file
        if (line == "") continue; 
        // Catches weird non-spaced input.
        else if (splitIndex<=0 || splitIndex>=(line.size()-1))
            throw runtime_error("Error parsing DNL configuration file (splitIndex).\n\n");
        cout << splitIndex << '\n';
        token = line.substr(0, splitIndex);
        cout << token << '\n';
        // Should be the multiplicity
        if (sscanf(token.c_str(), "%d", &mult) == 1) {
            cout << mult << '\n';
        }
        else throw runtime_error("Error parsing DNL configuration file (mult).\n\n");
        token = line.substr(splitIndex+1, line.size());
        cout << token << '\n';
        if (sscanf(token.c_str(), "%lf", &amp) == 1) {
            cout << amp << '\n';
        }
        else throw runtime_error("Error parsing DNL configuration file (amp).\n\n");
    }
    f.close();
    return;
}
