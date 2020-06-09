#define StatsStudy_cxx
#include "StatsStudy.h"

using namespace std;

// To study the sigma of residuals in different bins.

StatsStudy::StatsStudy(vector<Residual>* _residuals,
                       DetectorGeometry* _g,
                       PlotManager* _pm) :
                       residuals(_residuals), g(_g), pm(_pm) {}

// Initialize residual TH1Fs, separated by the x-y space they cover
// The bin widths determine both the number of plots
// and the area of x-y space they cover, sorting residuals in 2D.
void StatsStudy::InitializeResidualTH1Fs(
  Int_t xBinWidth, Int_t yBinWidth) {
    wx = xBinWidth; wy = yBinWidth; // Initialize members
    pair<Double_t, Double_t> xlims = g->GetModuleLimitsX();
    pair<Double_t, Double_t> ylims = g->GetModuleLimitsY();
    Int_t nbinsx = ceil((xlims.second - xlims.first)/wx);
    Int_t nbinsy = ceil((ylims.second - ylims.first)/wy);
    Double_t xBins[nbinsx + 1];
    Double_t yBins[nbinsy + 1];
    Int_t i = 0;
    Int_t j = 0;
    // highest x bin width not controlled
    for (i=0; i<nbinsx; i++) {
        xBins[i] = xlims.first + i*wx;
    }
    xBins[nbinsx] = xlims.second;
    // lowest y bin width not controlled
    // Filling array from top down
    for (i=0; i<nbinsy; i++) {
        yBins[nbinsy - i] = ylims.second - i*wy;
    }
    yBins[0] = ylims.first;
 
    vector<Combination> combVec = combinationVector();
    string name; string title;
    string xlow, xhigh, ylow, yhigh;
    for (auto combo=combVec.begin(); combo!=combVec.end(); combo++) {
        for (i=0; i<nbinsx; i++) {
            for (j=0; j<nbinsy; j++) {
                // Get bin limits as strings
                xlow = Tools::CStr(xBins[i], 2); 
                xhigh = Tools::CStr(xBins[i+1],2);
                ylow = Tools::CStr(yBins[i],2);
                yhigh = Tools::CStr(yBins[i+1],2);
                name = combo->String() + "_for_residuals_in_X" + 
                       to_string(i) + "_Y" + to_string(j);
                title = "#splitline{Combo: "+combo->String()+"}{";
                title+= "x#in["+xlow+","+xhigh+"] mm, ";
                title+= "y#in["+ylow+","+yhigh+"] mm}";
                // start ~with tgc_analysis config for residual plots
                pm->Add(name, title, 200, -5, 5, myTH1F);
            }
        }
    }
}
/*void StatsStudy::FillResidualTH1Fs() {
    string plotName;
    Int_t xBinNum, yBinNum; // access correct histogram by bin num.
    for (auto r=residuals->begin(); r!=residuals.end(); r++) {
        
    }

}*/
