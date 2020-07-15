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
    // if not, only 1 bin within geometric limits
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
    // NOTE: No way to deal with overlap of spec. x ray pt bins
    // For each x ray point, add rectangular bin
    // First, copy into other arrays so sorting doesn't destroy
    // original order
    vector<Double_t> xs, ys;
    copy(data->xnoms.begin(), data->xnoms.end(), back_inserter(xs));
    copy(data->ynoms.begin(), data->ynoms.end(), back_inserter(ys));
    
    // Sort
    // xnoms were sorted in XRayData constructor so this shouldn't do anything
    sort(xs.begin(), xs.end());
    sort(ys.begin(), ys.end());
    // Create x binning. For overlapping x values
    // (within a bin width of each other), 
    // center bin around their avg position.
    UShort_t itcount = 0;
    Double_t xavg = 0;
    UShort_t i = 0;    
    while (i < xs.size()) {
        // cout << "Outer " << xs.at(i) << '\n';
        xavg = xs.at(i);
        itcount = 1;
        // If you're at the end, stop.
        if (i == xs.size() - 1) {
            // cout << "Outer - don't check next\n";
            // DO THE BINNING FOR THE LAST PT
            DefineXRayBin(xavg, xlims.first, xlims.second, wx, "x");
            i++;
            continue;
        }
        while (abs(xs.at(i) - xs.at(i+1)) < wx) {
            // cout << "Inner " << xs.at(i) << ' ' << xs.at(i+1) << '\n';
            xavg += xs.at(i+1);
            itcount += 1;
            i++;
            // If you're at the end, stop.
            if (i == xs.size() - 1) {
                // cout << "Inner - incremented til the end\n";
                i++;
                break;
            }
        }
        // Get the average x
        xavg = xavg/itcount;
        // cout << "Average " << xavg << ' ' << itcount << '\n';
        // DO THE BINNING
        DefineXRayBin(xavg, xlims.first, xlims.second, wx, "x");
        // Increment to look at next point
        i++;
        // cout << "\n\n";
    }
   
    // Same for y
    itcount = 0;
    Double_t yavg = 0;
    i = 0;    
    while (i < ys.size()) {
        // cout << "Outer " << ys.at(i) << '\n';
        yavg = ys.at(i);
        itcount = 1;
        // If you're at the end, stop.
        if (i == ys.size() - 1) {
            // cout << "Outer - don't check next\n";
            // DO THE BINNING FOR THE LAST PT
            DefineXRayBin(yavg, ylims.first, ylims.second, wy, "y");
            i++;
            continue;
        }
        while (abs(ys.at(i) - ys.at(i+1)) < wy) {
            // cout << "Inner " << ys.at(i) << ' ' << ys.at(i+1) << '\n';
            yavg += ys.at(i+1);
            itcount += 1;
            i++;
            // If you're at the end, stop.
            if (i == ys.size() - 1) {
                // cout << "Inner - incremented til the end\n";
                i++;
                break;
            }
        }
        // Get the average y
        yavg = yavg/itcount;
        // cout << "Average " << yavg << ' ' << itcount << '\n';
        // DO THE BINNING
        DefineXRayBin(yavg, ylims.first, ylims.second, wy, "y");
        // Increment to look at next point
        i++;
        // cout << "\n\n";
    }
 
    // Finish off member initialization 
    xBinEdges.push_back(xlims.second);
    yBinEdges.push_back(ylims.second);
    nBinsX = xBinEdges.size() - 1;
    nBinsY = yBinEdges.size() -1;
    name = "xray_min_xwidth_" + to_string(wx) + "mm_min_ywidth_";
    name += to_string(wy) + "mm";
    // Code to print things
    /*for (auto x=data->xnoms.begin(); x!=data->xnoms.end(); x++)
        cout << *x << ' ';
    cout << "\n\n";
    for (auto x=xs.begin(); x!=xs.end(); x++)
        cout << *x << ' ';
    cout << "\n\n";
    for (auto y=data->ynoms.begin(); y!=data->ynoms.end(); y++)
        cout << *y << ' ';
    cout << "\n\n";
    for (auto y=ys.begin(); y!=ys.end(); y++)
        cout << *y << ' ';
    cout << "\n\n";*/

}

void Binning::DefineXRayBin(Float_t q, Float_t qmin, Float_t qmax,
                             Int_t width, string xOrY) {
    if ((xOrY != "x") && (xOrY != "y")) {
        cout << "Warning: Invalid string argument, " << xOrY;
        cout << ",  in Binning::DefineXRayBin. ";
        cout << "Valid strings are \"x\" or \"y\""; 
        cout << "(Called from Binning constructor).\n";
        return;
    }
    Float_t binLimLow, binLimHigh;
    // Defining and adding lower bin limit
    binLimLow = q - width/2;
    // If low limit is out of range, don't add it to Edges vector
    // Note: because of averaging x-ray points nearby,
    // don't have to deal with case where pt lower boundary < 
    // previous pt upper boundary
    if (binLimLow > qmin) {
        if (xOrY == "x") xBinEdges.push_back(binLimLow);
        else yBinEdges.push_back(binLimLow); // if not x it must be y
    }
    /*if (binLimLow < qmin) {
        binLimLow = qmin;
        // Don't add to Edges
    }
    else {
        if (xOrY == "x") xBinEdges.push_back(binLimLow);
        else yBinEdges.push_back(binLimLow); // else push to y
    }*/

    // Defining and adding upper bin limit
    binLimHigh = q + width/2;
    // Similarly to lower limit,
    if (binLimHigh < qmax) {
        // Add to Edges
        if (xOrY == "x") xBinEdges.push_back(binLimHigh);
        else yBinEdges.push_back(binLimHigh);
    }
    return;
}
