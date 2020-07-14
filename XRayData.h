#ifndef XRayData_h
#define XRayData_h
// Class that holds relevant x-ray data for this analysis
// read in data from sqlite 3 database
// Constructor fills members, to be used in other analyses
// Also parses RUN_ID field of x-ray data to get appropriate wedge
// and run

// C++ includes
#include <iostream>
#include <string>
#include <sqlite3.h>
#include <map>
#include <vector>

// ROOT includes
#include <TCanvas.h>
#include <TGraph.h>
#include <TAxis.h>

// tgc_analysis includes
#include "AnalysisInfo.h"

// My includes
#include "Helper.h"

class XRayData {
  public:
    // Constructors
    XRayData(){};
    XRayData(std::string databaseName, AnalysisInfo* _cinfo,
             InputInfo* _myInfo); 
    ~XRayData(){};

    // Members
    // Following three are ordered by distinct xray data positions
    std::vector<Double_t> xnoms;
    std::vector<Double_t> ynoms;
    std::vector<std::map<UShort_t, Double_t>> offsets; // layer to offset map
   // Methods
   // Plot ynoms vs xnoms, nominal xray positions
   void PlotPositions();

  private:
    AnalysisInfo* cinfo = nullptr;
    InputInfo* myInfo = nullptr;
    //parseRunID(string runId);
};
#endif
