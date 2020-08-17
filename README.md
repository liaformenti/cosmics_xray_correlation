# strip_position_analysis

Analysis tools to compare cosmics data with [x-ray wedge alignment data](https://twiki.cern.ch/twiki/bin/viewauth/Atlas/XRayWedgeAlignment). To bypass cosmics data having no fixed reference frame, differences in x-ray offsets (y_jigcmm_holdercmm - y_meas) between layers are compared to the difference in the mean of residuals in an area around where xray data was taken.

Usage:
./StripPosAnalysis CosmicsAnalysis.root QUADNAME outDirectory/

Strip position analysis takes in a CosmicsAnalysis.root file with a Tracks TTree. The quad under investigation must be supplied, in a format like, eg. QS3P06. The relative or absolute path to an output directory should also be provided.

RunAnalysis.cpp calls the various parts of the analysis. Since currently the analysis method is currently not fixed, it is frequently edited. The necessary and possible sections of the analysis are described below.

## 1) Tracking
StripPosAnalysis redoes the tracking using the data in the Tracks TTree with each permutation of two fixed layers, extrapolating to the remaining layers. A set of fixed layers provides the reference frame in which to define the misalignments.
The residuals for each track on the layers it was extrapolated / interpolated to are stored.

## 2) Getting XRayData
The XRayData constructor reads the sqlite3 database results.db, which should be located in the same directory as the source code (for now).

## 3) Comparing residuals data from tracking and XRayData
There are two comparison methods.

**a)** For each xray point, input an area of interest around the point (a width in x and y). Take the mean of residuals of tracks that fell in that region of interest. Compare the difference in xray offsets and mean residuals, using a method that outputs the results to a table.

**b)** Create a binning over the quad surface using the Binning class (need more options on how to construct bins). Create histograms of the quantities of interest to see differences over the layers' surfaces. Note that these plots can be made without reference to the xray data as well.
