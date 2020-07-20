#define CombinedData_cxx
#include "CombinedData.h"

using namespace std;

// Region of interest is rectange of x (y) width of wx (wy).
// Use flag outOfRange to indicate if bin goes outside module limits
CombinedData::CombinedData(Int_t xWidth, Int_t yWidth, Double_t _x, 
                           Double_t _y, UShort_t _lc, UShort_t _ld,
                           Double_t _offC, Double_t _offD,
                           vector<Residual>* _resData,
                           DetectorGeometry* _g, PlotManager* _pm) :
    x(_x), y(_y), lc(_lc), ld(_ld), offC(_offC), offD(_offD), 
    resData(_resData), g(_g), pm(_pm) {
    UShort_t la, lb;
    // Note that lc < ld always
    getOtherLayers(lc, ld, &la, &lb);
    // Note that offC, smaller-value layer, is first.
    offDiff = offC - offD;
    // Fill ROI member
    // X lims is first entry, y lims is second.
    DefineRectangularROI(xWidth, yWidth);
    cout << "x in [" << ROI[0].first << ", " << ROI[0].second << "]\n";
    cout << "y in [" << ROI[1].first << ", " << ROI[1].second << "]\n";

    
    // Determine which residuals are in ROI 
    for (auto r=resData->begin(); r!=resData->end(); r++) {
        // If not on right layer
        if ((lc != r->l) && (ld != r->l)) continue;
        // If fixed layers are not right,
        if ((la != r->la) || (lb != r->lb)) continue;
        // If residual is in ROI
        if ( (ROI[0].first < r->x) && (r->x < ROI[0].second) &&
             (ROI[1].first < r->y) && (r->y < ROI[1].second)) {
           if (r->l == lc) {
               residualsInROIC.push_back(r->res);
               // cout << r->res << ' ' << r->l << ' ' << r->x << ' ' << r->y << ' ' << r->la << ' ' << r->lb << '\n';
           }
           else if (r->l == ld) {
               residualsInROID.push_back(r->res);
               // cout << r->res << ' ' << r->l << ' ' << r->x << ' ' << r->y << ' ' << r->la << ' ' << r->lb << '\n';
           }
           else {
               cout << "Warning: Logic error in CombinedData width";
               cout << "-defined rectangular ROI constructor.\n\n";
           }
        }
    }
    return;
}

// This function fills lims, declared externally
void CombinedData::DefineRectangularROI(Int_t wx, Int_t wy) {
    // Define xlims
    ROI[0].first = x - wx/2.0;
    ROI[0].second = x + wx/2.0;
    // Define ylims
    ROI[1].first = y - wy/2.0;
    ROI[1].second = y + wy/2.0;
    return;
}
