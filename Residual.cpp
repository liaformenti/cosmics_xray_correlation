// Residual object
#define Residual_cxx
#include "Residual.h"

using namespace std;

// Should do hit - track
Residual::Residual(Double_t residual, UShort_t layer,
                   Double_t xpos, Double_t ypos,
                   UShort_t fixedLayer1, UShort_t fixedLayer2) {
    // Initialize members
    res = residual;
    l = layer;
    x = xpos;
    y = ypos;
    la = fixedLayer1;
    lb = fixedLayer2;
    // cout << "Constr.1 " << res << ' ' << l << ' ' << x << ' ' << y << ' ' << la << ' ' << lb << '\n';
}
Residual::Residual(Tracking &trackInfo, UShort_t layer) {
    l = layer;
    x = trackInfo.fitXPos[l];
    y = trackInfo.fitYPos[l];
    la = trackInfo.la;
    lb = trackInfo.lb;
    res = trackInfo.hitsY[l] - trackInfo.fitYPos[l];
    // cout << "Constr.2 " << res << ' ' << l << ' ' << x << ' ' << y << ' ' << la << ' ' << lb << '\n';
}
// Returns combination struct based on conditions in which residual
// was calculated.
Combination Residual::GetCombo() {
    return Combination(l, la, lb);
}

void Residual::PrintResidual() {
    cout << "Layer: " << l << ", fixed layers: " << la << lb << ", at x = " << x << " mm, y = " << y;
    cout << " mm, residual: " << res << '\n';
    return;
}
