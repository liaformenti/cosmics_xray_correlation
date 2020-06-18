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
void ResPlots::InitializeNumEntriesTH2Is(string nameBase) {
    string name;
    vector<Combination> combVec = combinationVector();    
    for (auto combo=combVec.begin(); combo!=combVec.end(); combo++) {
        name = nameBase + combo->String();
        // pm->Add(name, name, binning->nBinsX, binning->xBinEdges, 
        //        binning->nBinsY, binning->yBinEdges, myTH2I); 
    }
    return;
}
