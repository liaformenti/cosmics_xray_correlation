#define StatsStudy_cxx
#include "StatsStudy.h"

using namespace std;

// To study bin sizes regardless of x-ray position

StatsStudy::StatsStudy(std::vector<Residual>* _residuals,
               DetectorGeometry* _g,
               PlotManager* _pm) : 
               residuals(_residuals), pm(_pm), g(_g) {
    cout << "constructed\n";
    cout << g->GetZPosition(4) << '\n';
    pm->PrintAll();
    for (auto r=residuals->begin(); r!=residuals->end(); r++) {
        cout << r->res << ' ';
    }

}

