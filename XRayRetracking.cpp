#define XRayRetracking_cxx
#include "XRayRetracking.h"

using namespace std;

XRayRetracking::XRayRetracking(XRayData* _xData, AnalysisInfo* _cInfo, InputInfo* _myInfo, 
                               PlotManager* _pm, DetectorGeometry* _g) : Retracking(_cInfo,
                               _myInfo, _pm, _g), xData(_xData) {}

void XRayRetracking::Retrack() {

  // y-track values are offsets
  // x-track values are xbeams
  // z values are layers z positions
  
  Bool_t beans = 1;
  
  for (auto xPt=xData->pointVec.begin(); xPt!=xData->pointVec.end(); xPt++) {

    // xbeams do not have errors so create a map of zeros for them
    map<UShort_t, Double_t> uncertX; 
    for (auto x=xPt->xbeams.begin(); x!=xPt->xbeams.end(); x++) {
        uncertX[x->first] = 0; // mm
    }
    // For each set of fixed layers
    // Remember, la and lb are private members of class
    for (la=1; la<=4; la++) {
      for (lb=(la+1); lb<=4; lb++) {

        if (MissingHitsOnFixedLayers(xPt->xbeams, xPt->offsets)) 
          continue;

        getOtherLayers(la, lb, &lc, &ld);
        Tracking myTrack(g, pm, xPt->xbeams, uncertX, xPt->offsets, xPt->offsetErrors, la, lb);
        myTrack.Fit();

        // Calculate y-residuals on unfixed layers for this track and add to object's residuals vector
        Residual res;
        if (myTrack.hitsY.find(lc) != myTrack.hitsY.end()) {
            myTrack.EvaluateAt(lc);
            // Construct residual manually since in tracking y values were offsets
            res = Residual(myTrack.hitsY.at(lc) - myTrack.fitYPos.at(lc), lc, xPt->xbeams.at(lc), 
                           xPt->ybeams.at(lc), la, lb);
            res.tag = to_string(xPt->platformID) + xPt->positionNumber;
            residuals.push_back(res);
        }
        if (myTrack.hitsY.find(ld) != myTrack.hitsY.end()) {
            myTrack.EvaluateAt(ld);
            res = Residual(myTrack.hitsY.at(ld) - myTrack.fitYPos.at(ld), ld, xPt->xbeams.at(ld), 
                           xPt->ybeams.at(ld), la, lb);
            res.tag = to_string(xPt->platformID) + xPt->positionNumber;
            residuals.push_back(res);
        }
        
        if(beans) {
          myTrack.PlotFit("out/test_fit_2.pdf");
        }
        beans = 0;
      }
    } // End fixed layer combinations loop
  } // End xray point loop 
}

