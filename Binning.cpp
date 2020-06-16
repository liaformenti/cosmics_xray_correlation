#define Binning_cxx
#include "Binning.h"

using namespace std;

// Constructors all fill map of bin edges by layer

// Constructor for rectangular bins
// Lowest y and highest x bin widths not controlled
// This is so most bins will have exact widths with minimal harm
// to bin quality.
Binning::Binning(Int_t xWidth, Int_t yWidth, DetectorGeometry* _g) :
               xw{xWidth}, yw{yWidth}, g(_g) { 
    pair<Double_t, Double_t> xlims = g->GetModuleLimitsX();
    pair<Double_t, Double_t> ylims = g->GetModuleLimitsY();
    Int_t nbinsx = ceil((xlims.second - xlims.first)/xw);
    Int_t nbinsy = ceil((ylims.second - ylims.first)/yw);
    // Float_t xBins[nbinsx + 1];
    // Float_t yBins[nbinsy + 1];
    vector<Float_t> xBins;
    vector<Float_t> yBins;
    Int_t i = 0;
    // highest x bin width not controlled
    for (i=0; i<nbinsx; i++) {
        // xBins[i] = xlims.first + i*xw;
        xBins.push_back(xlims.first + i*xw);
    }
    // xBins[nbinsx] = xlims.second;
    xBins.push_back(xlims.second);
    // lowest y bin width not controlled
    // Filling array from top down
    for (i=0; i<nbinsy; i++) {
        // yBins[nbinsy - i] = ylims.second - i*yw;
        // yBins.push_back(ylims.second -i*w);
        yBins.insert(yBins.begin(), ylims.second - i*yw);
    }
    yBins.insert(yBins.begin(), ylims.first);
    // yBins[0] = ylims.first;
    // xBinEdges.insert(pair<UShort_t, Float_t*>(1, xBins)); 
    for (i=1; i<=4; i++) { // Fill map
        xBinEdges.insert(pair<UShort_t, vector<Float_t>>(i, xBins)); 
        yBinEdges.insert(pair<UShort_t,vector<Float_t>>(i, yBins));
    }
}
