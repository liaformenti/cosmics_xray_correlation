#ifndef CombinedData_h 
#define CombinedData_h

// C++ includes
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <sstream>

// tgc_analysis includes
#include "Tools.h"

// My includes
#include "XRayData.h"
#include "Helper.h"
#include "Residual.h"

// Combined data is a class which examines an xray point on two
// different layers, and selects and processes the corrresponding
// residuals data for that xray point.
// The elements to be compared are the difference in xray offsets
// between layers vs the difference in the means of the residuals
// that occured around that xray point

// PtLayerData contains the layer specific information required for
// the CombinedData analysis. It is useful for storing the layer spec
// info in map in the CombinedData class, keyed by each of the layers
// that are being compared. 
// Maybe eventually add an outOfRange member indicating if the class
// ROI reaches beyond a layer edge.
struct PtLayerData {
    // xray data elements
    // List of residuals that fall into region of interest
    std::vector<Double_t> residualsInROI;
    Double_t offset;
    Double_t offsetError;
    // Residual data elements
    // Mean and sigma from fit, based on Gaussian but could be filled
    // with any fit's metric of "centre" and "width"
    // Also stores whether fit was successful
    Bool_t success;
    Double_t mean;
    Double_t meanError;
    Double_t sigma;
    Double_t sigmaError;
    // Your prev. data analysis showed amplitude was best "goodness of 
    // fit" qualifier
    Double_t amplitude; 
    // Maybe add: Bool_t ROIOutOfRange
};

class CombinedData {
  public:
    // Constructors
    CombinedData(){};
    // Constructor takes in an xray point and the two layers you want
    // to analyze the difference of. Note the role reversal of la and
    // lb from being fixed layers to layers of interest
    CombinedData(XRayPt xPt, UShort_t layerA, UShort_t layerB, 
        std::vector<Residual>* _resData, DetectorGeometry* _g,
        PlotManager* _pm);
    ~CombinedData(){}; 

    // Members
    // Xray point's index (for naming plots)
    Int_t xPtIndex; 
    Double_t x, y; // Position of xray point 
    // Upper and lower limits of rectange of tgc_analysis coordinate
    // space around xray point. We compare xray offset differences
    // to the mean of residuals that fall in this ROI.
    std::pair<Double_t, Double_t> xROI, yROI;
    // Layers of interest
    UShort_t la, lb;
    // Residuals must be calculated with these layers fixed to study
    // layers of interest. Notice name reversal of la lb vs lc ld 
    // compared to Tracking.
    UShort_t lc, ld;
    // Map of layer of interest specific data used to generate results
    std::map<UShort_t, PtLayerData> layerData;
    // And the actual results we want:
    // Difference in xray offsets on two layers
    Double_t offDiff, offDiffError;
    Double_t meanDiff, meanDiffError;
   
    // Methods
    // To define a specified width of rectangle around the x ray pt
    // Widths are in mm
    void DefineRectangularROI(Double_t xWidth, Double_t yWidth);
    // Loop residuals and add those in ROIs to residualsInROIs members
    // of PtLayerData
    void FillROIsWithResiduals();
    void FitGaussian();
  private:
    std::vector<Residual>* resData = nullptr;
    DetectorGeometry* g = nullptr;
    PlotManager* pm = nullptr;
};
#endif
