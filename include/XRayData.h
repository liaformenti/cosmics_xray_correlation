#ifndef XRayData_h
#define XRayData_h
// Class that holds relevant x-ray data for this analysis
// read in data from sqlite 3 database
// Constructor fills members, to be used in other analyses
// Also parses RUN_ID field of x-ray data to get appropriate wedge
// and run

// XRayData.cpp is the file you need to edit if the format of the x-ray data changes or you want to change
// the selection of which x-ray measurements to use.

// C++ includes
#include <iostream>
#include <string>
#include <sqlite3.h>
#include <map>
#include <vector>
#include <fstream>
#include <sstream>

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
    Int_t platformID;
    std::string positionNumber;
    // Keys are layers
    std::map<UShort_t, Double_t> xbeams;
    std::map<UShort_t, Double_t> ybeams;
    std::map<UShort_t, Double_t> offsets;
    std::map<UShort_t, Double_t> offsetErrors;
    std::vector<std::pair<UShort_t, UShort_t>> GetDiffCombos();
};

class XRayData {
  public:
    // Constructors
    XRayData(){};
    XRayData(AnalysisInfo* _cinfo, InputInfo* _myInfo, PlotManager* _pm); 
    ~XRayData(){};

    // Members
    std::vector<XRayPt> pointVec;

    // Methods
    // Filepath indicates the path and name of file to hold the resulting database
    // void csv2db(std::string inFileName, std::string outFileName);
    // Fills platformID and positionNumber with approrpriate strings from runID
    void ParseRunID(std::string runID, std::string& platformID, std::string& positionNumber);
    // Plot ybeams vs xbeams, nominal xray positions
    // Take average of position over available layers.
    void PlotAverageBeamPositions();
    // Writes out positions and offsets for each available layer
    void WriteOutXRayData();
    // For an offsets entry, return all combinations of layers which
    // have an offset for further processing

  private:
    AnalysisInfo* cinfo = nullptr;
    InputInfo* myInfo = nullptr;
    PlotManager* pm = nullptr;
    // Quad to wedge map
    // Information parsed from wedge_parenting.txt from stgc-as-built-fit on 2021-11-12
    const std::map<std::string,std::string> quadToWedge = {
    {"QS1P08", "20MNIWSAP00003"},
    {"QS2P04", "20MNIWSAP00003"},
    {"QS3P01", "20MNIWSAP00003"},
    {"QS2P07", "20MNIWSAP00002"},
    {"QS1P04", "20MNIWSAP00002"},
    {"QS3P06", "20MNIWSAP00002"},
    {"QS1P02", "20MNIWSAP00001"},
    {"QS2P00", "20MNIWSAP00001"},
    {"QS3P02", "20MNIWSAP00001"},
    {"QS1P10", "20MNIWSAP00004"},
    {"QS2P10", "20MNIWSAP00004"},
    {"QS3P08", "20MNIWSAP00004"},
    {"QS1C02", "20MNIWSAC00002"},
    {"QS3C02", "20MNIWSAC00002"},
    {"QS2C13", "20MNIWSAC00002"},
    {"QS2C09", "20MNIWSAC00003"},
    {"QS1C03", "20MNIWSAC00003"},
    {"QS3C04", "20MNIWSAC00003"},
    {"QS2C10", "20MNIWSAC00001"},
    {"QS1C01", "20MNIWSAC00001"},
    {"QS3C03", "20MNIWSAC00001"},
    {"QS1P09", "20MNIWSAP00008"},
    {"QS3P10", "20MNIWSAP00008"},
    {"QS2P06", "20MNIWSAP00008"},
    {"QS2P05", "20MNIWSAP00005"},
    {"QS1P07", "20MNIWSAP00005"},
    {"QS3P07", "20MNIWSAP00005"},
    {"QS3C10", "20MNIWSAC00004"},
    {"QS1C04", "20MNIWSAC00004"},
    {"QS2C15", "20MNIWSAC00004"},
    {"QS1C06", "20MNIWSAC00005"},
    {"QS2C16", "20MNIWSAC00005"},
    {"QS3C11", "20MNIWSAC00005"},
    {"QL3P04", "20MNIWLAP00001"},
    {"QL1P07", "20MNIWLAP00001"},
    {"QL2P01", "20MNIWLAP00001"},
    {"QS2C12", "20MNIWSAC00007"},
    {"QS1C07", "20MNIWSAC00007"},
    {"QS3C08", "20MNIWSAC00007"},
    {"QL1P05", "20MNIWLAP00003"},
    {"QL2P03", "20MNIWLAP00003"},
    {"QL3P03", "20MNIWLAP00003"},
    {"QS2C00", "20MNIWSAC00006"},
    {"QS1C05", "20MNIWSAC00006"},
    {"QS3C12", "20MNIWSAC00006"},
    {"QL3P02", "20MNIWLAP00002"},
    {"QL2P02", "20MNIWLAP00002"},
    {"QL1P09", "20MNIWLAP00002"},
    {"QS1C08", "20MNIWSAC00008"},
    {"QS2C11", "20MNIWSAC00008"},
    {"QS3C07", "20MNIWSAC00008"},
    {"QL1P10", "20MNIWLAP00004"},
    {"QL3P07", "20MNIWLAP00004"},
    {"QL2P04", "20MNIWLAP00004"},
    {"QS2P08", "20MNIWSAP00009"},
    {"QS1P12", "20MNIWSAP00009"},
    {"QS3P09", "20MNIWSAP00009"},
    {"QL3C05", "20MNIWLAC00003"},
    {"QL2C02", "20MNIWLAC00003"},
    {"QL1C01", "20MNIWLAC00003"},
    {"QL3C02", "20MNIWLAC00002"},
    {"QL1C03", "20MNIWLAC00002"},
    {"QL2C04", "20MNIWLAC00002"},
    {"QL2P06", "20MNIWLAP00006"},
    {"QL1P06", "20MNIWLAP00006"},
    {"QL3P08", "20MNIWLAP00006"},
    {"QL3P09", "20MNIWLAP00005"},
    {"QL1P12", "20MNIWLAP00005"},
    {"QL2P05", "20MNIWLAP00005"},
    {"QL2C01", "20MNIWLAC00001"},
    {"QL3C06", "20MNIWLAC00001"},
    {"QL1C02", "20MNIWLAC00001"},
    {"QL3C07", "20MNIWLAC00004"},
    {"QL1C07", "20MNIWLAC00004"},
    {"QL2C03", "20MNIWLAC00004"},
    {"QL3P06", "20MNIWLAP00007"},
    {"QL1P13", "20MNIWLAP00007"},
    {"QL2P07", "20MNIWLAP00007"},
    {"QL3C08", "20MNIWLAC00005"},
    {"QL2C05", "20MNIWLAC00005"},
    {"QL1C10", "20MNIWLAC00005"},
    {"QS3P03", "20MNIWSCP00001"},
    {"QS1P15", "20MNIWSCP00001"},
    {"QS2P15", "20MNIWSCP00001"},
    {"QL3C10", "20MNIWLCC00001"},
    {"QL1C05", "20MNIWLCC00001"},
    {"QL2C09", "20MNIWLCC00001"},
    {"QL2P08", "20MNIWLAP00008"},
    {"QL1P14", "20MNIWLAP00008"},
    {"QL3P05", "20MNIWLAP00008"},
    {"QL1P08", "20MNIWLCP00001"},
    {"QL3P10", "20MNIWLCP00001"},
    {"QL2P09", "20MNIWLCP00001"},
    {"QL3C01", "20MNIWLAC00007"},
    {"QL2C08", "20MNIWLAC00007"},
    {"QL1C11", "20MNIWLAC00007"},
    {"QL1C08", "20MNIWLAC00006"},
    {"QL2C06", "20MNIWLAC00006"},
    {"QL3C03", "20MNIWLAC00006"},
    {"QL1C09", "20MNIWLAC00008"},
    {"QL2C07", "20MNIWLAC00008"},
    {"QL3C11", "20MNIWLAC00008"},
    {"QS3P11", "20MNIWSAP00010"},
    {"QS1P13", "20MNIWSAP00010"},
    {"QS2P11", "20MNIWSAP00010"},
    {"QS2C02", "20MNIWSAC00009"},
    {"QS1C09", "20MNIWSAC00009"},
    {"QS3C06", "20MNIWSAC00009"},
    {"QS2P13", "20MNIWSAP00011"},
    {"QS3P12", "20MNIWSAP00011"},
    {"QS1P14", "20MNIWSAP00011"},
    {"QL1P04", "20MNIWLCP00002"},
    {"QL2P10", "20MNIWLCP00002"},
    {"QL3P14", "20MNIWLCP00002"},
    {"QS1P17", "20MNIWSCP00002"},
    {"QS3P13", "20MNIWSCP00002"},
    {"QS2P16", "20MNIWSCP00002"},
    {"QS2P20", "20MNIWSCP00003"},
    {"QS3P14", "20MNIWSCP00003"},
    {"QS1P16", "20MNIWSCP00003"},
    {"QL1C17", "20MNIWLCC00003"},
    {"QL3C17", "20MNIWLCC00003"},
    {"QL2C11", "20MNIWLCC00003"},
    {"QL3P13", "20MNIWLCP00003"},
    {"QL1P16", "20MNIWLCP00003"},
    {"QL2P11", "20MNIWLCP00003"},
    {"QL2C12", "20MNIWLCC00004"},
    {"QL1C16", "20MNIWLCC00004"},
    {"QL3C15", "20MNIWLCC00004"},
    {"QL1C12", "20MNIWLCC00005"},
    {"QL2C13", "20MNIWLCC00005"},
    {"QL3C14", "20MNIWLCC00005"},
    {"QL3P12", "20MNIWLCP00004"},
    {"QL2P12", "20MNIWLCP00004"},
    {"QL1P18", "20MNIWLCP00004"},
    {"QL1C14", "20MNIWLCC00002"},
    {"QL3C09", "20MNIWLCC00002"},
    {"QL2C10", "20MNIWLCC00002"},
    {"QS2C17", "20MNIWSCC00001"},
    {"QS3C16", "20MNIWSCC00001"},
    {"QS1C10", "20MNIWSCC00001"},
    {"QS2P17", "20MNIWSCP00004"},
    {"QS3P15", "20MNIWSCP00004"},
    {"QS1P19", "20MNIWSCP00004"},
    {"QL2P13", "20MNIWLCP00005"},
    {"QL1P15", "20MNIWLCP00005"},
    {"QL3P15", "20MNIWLCP00005"},
    {"QS3C14", "20MNIWSCC00003"},
    {"QS2C14", "20MNIWSCC00003"},
    {"QS1C51", "20MNIWSCC00003"},
    {"QS2P09", "20MNIWSCP00006"},
    {"QS3P17", "20MNIWSCP00006"},
    {"QS1P20", "20MNIWSCP00006"},
    {"QS2C06", "20MNIWSCC00006"},
    {"QS3C13", "20MNIWSCC00006"},
    {"QS1C53", "20MNIWSCC00006"},
    {"QL2P15", "20MNIWLCP00007"},
    {"QL1P19", "20MNIWLCP00007"},
    {"QL3P18", "20MNIWLCP00007"},
    {"QL1C06", "20MNIWLCC00006"},
    {"QL3C13", "20MNIWLCC00006"},
    {"QL2C14", "20MNIWLCC00006"},
    {"QS2C01", "20MNIWSCC00002"},
    {"QS3C15", "20MNIWSCC00002"},
    {"QS1C11", "20MNIWSCC00002"},
    {"QS3P16", "20MNIWSCP00005"},
    {"QS2P12", "20MNIWSCP00005"},
    {"QS1P18", "20MNIWSCP00005"},
    {"QL2C15", "20MNIWLCC00007"},
    {"QL3C16", "20MNIWLCC00007"},
    {"QL1C19", "20MNIWLCC00007"},
    {"QS2C03", "20MNIWSCC00005"},
    {"QS3C01", "20MNIWSCC00005"},
    {"QS1C12", "20MNIWSCC00005"},
    {"QS2P19", "20MNIWSCP00007"},
    {"QS3P18", "20MNIWSCP00007"},
    {"QS1P21", "20MNIWSCP00007"},
    {"QL2C17", "20MNIWLCC00008"},
    {"QL3C12", "20MNIWLCC00008"},
    {"QL1C15", "20MNIWLCC00008"},
    {"QS3C09", "20MNIWSCC00007"},
    {"QS2C19", "20MNIWSCC00007"},
    {"QS1C52", "20MNIWSCC00007"},
    {"QS3P04", "20MNIWSCP00009"},
    {"QS2P14", "20MNIWSCP00009"},
    {"QS1P22", "20MNIWSCP00009"},
    {"QL2P17", "20MNIWLCP00009"},
    {"QL1P20", "20MNIWLCP00009"},
    {"QL3P19", "20MNIWLCP00009"},
    {"QL1C18", "20MNIWLCC00009"},
    {"QL2C18", "20MNIWLCC00009"},
    {"QL3C19", "20MNIWLCC00009"},
    {"QS3C18", "20MNIWSCC00008"},
    {"QS2C05", "20MNIWSCC00008"},
    {"QS1C14", "20MNIWSCC00008"},
    {"QS2P18", "20MNIWSCP00008"},
    {"QS1P11", "20MNIWSCP00008"},
    {"QS3C17", "20MNIWSCP00008"},
    {"QL3P01", "20MNIWLCP00008"},
    {"QL1P02", "20MNIWLCP00008"},
    {"QL2P16", "20MNIWLCP00008"},
    {"QS3C19", "20MNIWSCC00004"},
    {"QS2C18", "20MNIWSCC00004"},
    {"QS1C13", "20MNIWSCC00004"},
    {"QS1C54", "20MNIWSCC00009"},
    {"QS2C08", "20MNIWSCC00009"},
    {"QS3P19", "20MNIWSCC00009"},
    {"QL1P11", "20MNIWLCP00006"},
    {"QL2P14", "20MNIWLCP00006"},
    {"QL3P16", "20MNIWLCP00006"}
    };
};
#endif
