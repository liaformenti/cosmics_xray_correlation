#include <iostream>
#include <fstream>
#include "DetectorGeometryTools.h"
#include <string.h>
#include <vector>
#include <TGraph.h>
#include <TMultiGraph.h>
#include <TCanvas.h>

using namespace std;


// For playing with geometry functions
// Should be compiled within tgc_analysis/CosmicsAnalysis with line on your PC
// g++ `root-config --cflags --glibs` `xml2-config --cflags` -lxml2 testGeom.cpp DetectorGeometryTools.cpp DetectorGeometry.cpp DetectorGeometryNSW.cpp Sector.cpp Tools.cpp

int main() {

    // ***** SELECT QUAD GEOM ***** //
    string geomStr = "QL2C";
    DetectorGeometry* g = DetectorGeometryTools::GetDetectorGeometry(geomStr);
    cout << g->GetModuleLimitsX().first << ' ' << g->GetModuleLimitsX().second << '\n';
    cout << g->GetModuleLimitsY().first << ' ' << g->GetModuleLimitsY().second << '\n';
 
    /********
    Print z positions of layers
    *********/
    /*for (int l=1; l<=4; l++) {
        cout << "Layer " << l << " z=" << g->GetZPosition(l) << '\n';
    }
    cout << g->GetZPosition(1) - g->GetZPosition(2) << '\n';
    cout << g->GetZPosition(2) - g->GetZPosition(3) << '\n';
    cout << g->GetZPosition(3) - g->GetZPosition(4) << '\n';
    */

    /*********
    Print wire support positions
    **********/
    /*vector<Double_t> supports;
    vector<Double_t> supportPos;
    for (int l=1; l<=4; l++) {
        cout << "Layer " << l << '\n';
        supports = g->GetSupportsY(l);
        for (auto s=supports.begin(); s!=supports.end(); s++) {
            cout << *s << ' ';
            supportPos.push_back(*s);
        }
        cout << "\n\n";
    }*/
    // Plot and print 
    /*TCanvas* c = new TCanvas();
    c->Print("wireSupportShadows.pdf(");
    Double_t x[2];
    x[0] = -500; // mm
    x[1] = 500; // mm
    Double_t y[2]; // mm 
    TGraph* graphs[20];
    TMultiGraph* mg = new TMultiGraph();
    for (int i=0; i<20; i++) {
        y[0] = supportPos.at(i);
        y[1] = supportPos.at(i);
        graphs[i] = new TGraph(2, x, y);
        mg->Add(graphs[i]);
    }
    mg->Draw("AL");
    mg->SetTitle(("Wire support shadows for " + geomStr + ";x [mm];y [mm]").c_str());
    c->Print("wireSupportShadows.pdf");
    c->Print("wireSupportShadows.pdf)");
    for (int i=0; i<20; i++) delete graphs[i];
    delete mg;
    delete c;*/

    /********
    y support widths
    *********/
    /*vector<Double_t> supportsWidths;
    for (int l=1; l<=4; l++) {
        supportsWidths = g->GetSupportsWidth(l);
        for (auto s=supportsWidths.begin(); s!=supportsWidths.end(); s++) {
            cout << *s << ' ';
        }
        cout << "\n\n";
    }*/

    /**********
    Position of strip elecID 200 
    **********/
    /*for (int l=1; l<=4; l++) {
         cout << g->PositionChannel(Channel(fSTRIP, l, 200)) << ' ';
    }*/
    /**********
    Check out the offset between pattern starts for strips and wires between layers
    Repeat for first channel and 200th channel (elecID)
    **********/
    /*ofstream f;
    f.open(geomStr + "_strips_position.txt");
    f << "Layer 1, 2, 3, 4\n";
    f << "First strip y position: ";
    for (UShort_t l=1; l<=4; l++) {
        f << g->PositionChannel(g->GetFirstChannel(Sector(fSTRIP, l))) << ", ";
    }
    f << '\n';
    f << "200th strip y position: ";
    for (UShort_t l=1; l<=4; l++) {
        f << g->PositionChannel(Channel(fSTRIP, l, 200)) << ", ";
    }
    f.close();*/

    /*for(UShort_t c=5; c<8; c++) {
        for (UShort_t l=1; l<=4; l++) {
            cout << g->PositionChannel(Channel(fWIRE, l, c)) << ' ';
        }
        cout << '\n';
    }
    for (UShort_t l=1; l<=4; l++) {
        cout << g->PositionChannel(g->GetFirstChannel(Sector(fSTRIP, l))) << ' ';
    }*/

    /**********
    Check out the wire pitch across channels for each layer
    **********/
    /*** First channels ***/
    /*for (UShort_t l=1; l<=4; l++) {
        for (UShort_t c=2; c<=10; c++) {
            cout << g->PositionChannel(Channel(fWIRE, l, c)) - g->PositionChannel(Channel(fWIRE, l, c-1)) << ' ';
        }
        cout << '\n';
    }
    cout << "\n\n";*/
    /*** Last channels ***/
    /* UShort_t lastCID; 
    for (UShort_t l=1; l<=4; l++) {
        lastCID = g->GetLastChannel(Sector(fWIRE, l)).electrode_ID; 
        for (UShort_t c=lastCID-1; c>lastCID-10; c--) {
            cout << g->PositionChannel(Channel(fWIRE, l, c+1)) - g->PositionChannel(Channel(fWIRE, l, c)) << ' ';
        }
        cout << '\n';
    }*/

    /**********
    Make figure
    **********/
}
