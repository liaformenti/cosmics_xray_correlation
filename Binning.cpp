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
    // Set name
    name = "xbin_width_" + to_string(wx) + "mm_ybin_width_";
    name += to_string(wy) + "mm";
}

Binning::Binning(XRayData* data, Int_t wx, Int_t wy,
                DetectorGeometry* _g) : g(_g) {
    
    // Get geom limits
    pair<Double_t, Double_t> xlims = g->GetModuleLimitsX();
    pair<Double_t, Double_t> ylims = g->GetModuleLimitsY();

    // pushback lower lims
    xBinEdges.push_back(xlims.first);
    yBinEdges.push_back(ylims.first);
   
    // Check if there is xray data
    if (data->xnoms.size() == 0) {
        cout << "Warning: no xray data positions. One bin over entire area (Binning constructor).\n\n";
        xBinEdges.push_back(xlims.second);
        yBinEdges.push_back(ylims.second);
        nBinsX = 1; 
        nBinsY = 1;
        return;
    }

    // Should verify sizes are the same, maybe do this in XRayData
    //if (data->xnoms.size() != data->ynoms.size()) 
    /*for (Int_t i=0; i<data->xnoms.size(); i++) {
        cout << data->xnoms.at(i) << ' ' << data->ynoms.at(i) << '\n';
    }*/
    /*for(unsigned i : indices(data->xnoms)) {
        cout << data->xnoms.at(i) << ' ';
    }*/
    /*for (auto& [x, y] : zip(data->xnoms, data->ynoms)) {
        cout << x << ' ' << y << '\n';
    }
    cout << "\n\n";*/
}
