#define CombinedData_cxx
#include "CombinedData.h"

using namespace std;

CombinedData::CombinedData(Int_t wx, Int_t wy, Double_t _x, Double_t _y,
                           UShort_t _lA, UShort_t _lB,
                           Double_t _offA, Double_t _offB,
                           vector<Residual>* _resData,
                           DetectorGeometry* _g, PlotManager* _pm) :
    x(_x), y(_y), lA(_lA), lB(_lB), offA(_offA), offB(_offB), 
    resData(_resData), g(_g), pm(_pm) {
    cout << x << ' ' << y << ' ' << lA << ' ' << lB << ' ' << offA << ' ' << offB << '\n';
    return;
}
