#define Binning_cxx
#include "Binning.h"

using namespace std;

// Constructors all fill bin edges vector

// Constructor for rectangular bins, same for all layers.
// Lowest y and highest x bin widths not controlled
// This is so most bins will have exact widths with minimal harm
// to bin quality.
Binning::Binning(Int_t wx, Int_t wy, DetectorGeometry* _g) :
               g(_g) { 
    pair<Double_t, Double_t> xlims = g->GetModuleLimitsX();
    pair<Double_t, Double_t> ylims = g->GetModuleLimitsY();
    nBinsX = ceil((xlims.second - xlims.first)/wx);
    nBinsY = ceil((ylims.second - ylims.first)/wy);
    // Float_t xBins[nbinsx + 1];
    // Float_t yBins[nbinsy + 1];
    // vector<Float_t> xBins;
    // vector<Float_t> yBins;
    Int_t i = 0;
    // highest x bin width not controlled
    for (i=0; i<nBinsX; i++) {
        // xBins[i] = xlims.first + i*wx;
        xBinEdges.push_back(xlims.first + i*wx);
    }
    // xBins[nbinsx] = xlims.second;
    xBinEdges.push_back(xlims.second);
    // lowest y bin width not controlled
    // Filling array from top down
    for (i=0; i<nBinsY; i++) {
        // yBins[nbinsy - i] = ylims.second - i*wy;
        // yBins.push_back(ylims.second -i*w);
        yBinEdges.insert(yBinEdges.begin(), ylims.second - i*wy);
    }
    yBinEdges.insert(yBinEdges.begin(), ylims.first);
    // yBins[0] = ylims.first;
    // xBinEdges.insert(pair<UShort_t, Float_t*>(1, xBins)); 
    /*for (i=1; i<=4; i++) { // Fill map
        xBinEdges.insert(pair<UShort_t, vector<Float_t>>(i, xBins)); 
        yBinEdges.insert(pair<UShort_t,vector<Float_t>>(i, yBins));
    }*/
}
