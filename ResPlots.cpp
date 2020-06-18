#define ResPlots_cxx
#include "ResPlots.h"

using namespace std;

ResPlots::ResPlots(vector<Residual>* _residuals,
                   Binning* _binning,
                   DetectorGeometry* _g,
                   PlotManager* _pm) : residuals(_residuals),
                   binning(_binning), g(_g), pm(_pm) {}

void ResPlots::CreateNumEntriesTH2Is(string nameBase) {
    InitializeNumEntriesTH2Is(nameBase);
    pm->PrintAll();
}
// Note that bin edges vectors of binning are transfered to Double_t
// arrays for use with PlotManager methods.
void ResPlots::InitializeNumEntriesTH2Is(string nameBase) {
    string name;
    Double_t xEdges[binning->nBinsX + 1];
    copy(binning->xBinEdges.begin(), binning->xBinEdges.end(), xEdges);
    Double_t yEdges[binning->nBinsY + 1];
    copy(binning->yBinEdges.begin(), binning->yBinEdges.end(), yEdges);
    vector<Combination> combVec = combinationVector();    
    for (auto combo=combVec.begin(); combo!=combVec.end(); combo++) {
        name = nameBase + combo->String();
        pm->Add(name, name, binning->nBinsX, xEdges,
               binning->nBinsY, yEdges, myTH2I); 
    }
    return;
}
