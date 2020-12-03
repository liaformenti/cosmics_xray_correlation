// Class for comparing xray data residuals to cosmics residuals after retracking
// Loops through xray points and combinations. Takes the cosmics residuals within a width of
// the xray points and calculates the mean residual (Gaussian fit for now). Compares the mean cosmics
// residual to the xray residual.
// Nov 30, 2020

#ifndef CompareData_h
#define CompareData_h

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


// Holds the relevant xray and cosmics data for a specific xray point
class LocalData {
  public:
    //Constructors
    LocalData(){};
    // Constructor to set a fixed bin width for region of interest around local
    LocalData(Residual xrayResidual);
    ~LocalData(){};
   
    // Methods
    // Set regions of interest around xray points which define which cosmics residuals go into
    // calculation of mean residual. Units are mm.
    // Must set the ROIs somehow before grouping cosmics in them.
    void SetRectangularROIs(Double_t xWidth, Double_t yWidth); 
    // Loops throug the cosmics residuals and puts those within the ROI in the
    // cosmicsResiduals vector
    void GroupCosmicResiduals(const std::vector<Residual>& allCosmicResiduals);

  private:
    Residual xRes; // The xray residual at this point
    std::vector<Residual> cosmicResidualsInROI; // Fill with residuals that fall in ROI with correct combo
    std::pair<Double_t, Double_t> xROI, yROI; // Regions of interest around xray point
    // Some sort of fit data struct for cosmics Fit
};

class CompareData {

  public:

    // Constructors
    CompareData(){};
    // Bin widths are how far around xray point will cosmics be included in calculated mean
    // exclusive residual.
    CompareData(Double_t xBinWidth, Double_t yBinWidth, std::vector<Residual>* _xRayResiduals, 
                std::vector<Residual>* _cosmicsResiduals, InputInfo* _myInfo, PlotManager* _pm, 
                DetectorGeometry* _g);
    ~CompareData(){};

    // Members
    // Desired width of bin around xray point. Could also be used as a seed if you eventually do
    // custom widths depending on data availability. In mm.
    Double_t xWidth, yWidth; 
    // Vector holding local data for each xray point required to do comparison between xray and cosmics 
    // data
    std::vector<LocalData> localDataVec; 

    // Methods
    // Does main loop to group cResiduals about xray ponits, do fits, and store residual comparisons
    // in compPointVec
    void DoComparison();
    // Make mean cosmics exclusive residual vs x ray data residual
    void MakeScatterPlot();
    
  private:
    std::vector<Residual>* xResiduals = nullptr;
    std::vector<Residual>* cResiduals = nullptr;
    InputInfo* myInfo = nullptr;
    PlotManager* pm = nullptr;
    DetectorGeometry* g = nullptr;
};

#endif
