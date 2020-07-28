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
#include <fstream>

// ROOT includes
#include <TCanvas.h>
#include <TGraph.h>
#include <TAxis.h>

// tgc_analysis includes
#include "AnalysisInfo.h"
#include "PlotManager.h"

// My includes
#include "Helper.h"

struct XRayPt {
    Int_t num; // Unique identifying number for xray point, start at 0
    Double_t xnom;
    // Note: my ynom = y_jigcmm_holdercmm in xray data
    Double_t ynom; 
    std::map<UShort_t, std::string> dqFlags;
    // Keys are layers
    std::map<UShort_t, Double_t> offsets;
    std::map<UShort_t, Double_t> offsetErrors;
};

class XRayData {
  public:
    // Constructors
    XRayData(){};
    XRayData(std::string databaseName, AnalysisInfo* _cinfo,
            InputInfo* _myInfo, PlotManager* _pm); 
    ~XRayData(){};

    // Members
    std::vector<XRayPt> pointVec;
    // Following three are ordered by distinct xray data positions
    // std::vector<Int_t> nums; // Arbitrary xray pt index
    // std::vector<Double_t> xnoms;
    // std::vector<Double_t> ynoms;
    // std::vector<std::map<UShort_t, Double_t>> offsets; // layer to offset map
    // When init'ed, XRayData contains all XRayData pts.
    // User can choose to Cut x ray points that only have offset for
    // one layer, in which case Bool_t will be set to True.
    // Bool_t curated = false;
    // Methods
    // Plot ynoms vs xnoms, nominal xray positions
    void PlotPositions();
    // Writes out positions and offsets for each available layer
    void WriteOutXRayData();
    // For an offsets entry, return all combinations of layers which
    // have an offset for further processing
    // std::vector<std::pair<UShort_t, UShort_t>> GetDiffCombos(
    //    std::map<UShort_t, Double_t> theOffset);

  private:
    AnalysisInfo* cinfo = nullptr;
    InputInfo* myInfo = nullptr;
    PlotManager* pm = nullptr;
};
#endif
