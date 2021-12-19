# cosmics_xray_correlation

Analysis tools to compare cosmic muon data with [x-ray wedge alignment data](https://twiki.cern.ch/twiki/bin/viewauth/Atlas/XRayWedgeAlignment).

Main author: Lia Formenti, liaformenti\[at\]gmail.com 
Responsible: Brigitte Vachon, brigitte.vachon\[at\]cern.ch

# Background

For a complete background, see [Lia Formenti's MSc thesis](https://github.com/liaformenti/thesis).

Small strip thin gap chambers (sTGCs) were constructed for the ATLAS Experiment's New Small Wheels (NSWs). sTGCs are gas ionization chambers designed to detect muons. Four sTGCs are stacked into a quadruplet. Each layer of the quadruplet records information about the passage of a muon. Offline, the recorded data is processed with [tgc_analysis](https://gitlab.cern.ch/McGill-sTGC/tgc_analysis) to reconstruct muon tracks and tabulate other metrics used to characterize the performance of the quadruplet.

The position of the muon in the precision coordinate is recorded by strip electrodes on each layer, which have 3.2 mm pitch. During construction, it is possible that strip layers were shifted off of their nominal position. `cosmics_xray_correlation` uses track residuals to characterize the relative local offset of the strip pattern in regions of interest with respect to two reference layers. 

Offsets of the strip pattern cause a muon's recorded position to be shifted opposite to its real position. With cosmics data, there is no external reference frame to provide the real position, so only information about relative strip position offsets can be extracted. The reference frames used in `cosmics_xray_correlation` use the recorded muon positions on two fixed layers. Cosmic muon tracks are built using the hits on two fixed layers, and residuals are calculated on a third layer. For tracks falling in a region of interest, the mean of residuals on the third layer is calculated. The mean of residuals will be shifted systematically depending on the local relative offset of the strips on the layer of interest with respect to the local pattern of the strips on the two fixed layers, which in turn depends on the absolute misalignment of layers. The set of reference layers used to calculate the residual on the layer of interest is referred to as "the tracking combination."

The [x-ray method](https://twiki.cern.ch/twiki/bin/viewauth/Atlas/XRayWedgeAlignment) tabulates the local offset of the strip pattern at a limited number of locations over the surface of a quadruplet. The complete x-ray production run data is stored in directory, xrayData, in an SQL database, xray_results_2021-11-26.db. As of 2021-12-19, Stergios Tsigaridas (stergios.tsigaridas\[at\]cern.ch) and Isabel Trigger (isabel.trigger@\[at\]cern.ch) are responsible for the x-ray data.  
X-ray offsets (y_meas_raw_w - y_corrangle as labelled in the database) are used to mimick an "x-ray track" so x-ray residuals can calculated using the offsets on two fixed layers. The x-ray residuals can then be compared to the mean of cosmics residuals around them. The program repeats this process for all positions on the quadruplet where x-ray data was taken for all possible sets of reference layers. Several outputs characterizing the cosmics data and comparing the cosmics and x-ray data are generated.

## Getting cosmics_xray_correlation

```
git clone --recurse-submodules ssh://git@gitlab.cern.ch:7999/McGill-sTGC/cosmics_xray_correlation.git
```
Alternatively, you may clone using https protocol. The flag --recurse-submodules is required since cosmics_xray_correlation takes [tgc_analysis](https://gitlab.cern.ch/McGill-sTGC/tgc_analysis) as a submodule.

## Requirements

cosmics_xray_correlation and ReClustering (a program in a subdirectory, described later) have the same dependencies as [tgc_analysis](https://gitlab.cern.ch/McGill-sTGC/tgc_analysis), detailed in its README. To setup the appropriate environment on lxplus, run the following:

```
export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase
source ${ATLAS_LOCAL_ROOT_BASE}/user/atlasLocalSetup.sh
localSetupROOT 6.14.04-x86_64-slc6-gcc62-opt
export ALRB_rootVersion=6.14.04-x86_64-slc6-gcc62-opt
```

Other subdirectories (contents described later) contain various useful Python scripts. All Python code was written for `python3`.

## Compilation of C++ code

All modules and submodules have their own Makefile. So to compile, simply type "make".

## Data preparation

Raw cosmic muon data collected with [sTGC_readout_sw-master](https://gitlab.cern.ch/ssun/sTGC_readout_sw) must be [decoded](https://gitlab.cern.ch/McGill-sTGC/tgc_analysis/-/tree/master/vmm3decoder/Decoder) then run through [CosmicsAnalysis](https://gitlab.cern.ch/McGill-sTGC/tgc_analysis/-/tree/master/CosmicsAnalysis) with the saveTracks option set to "on" so that a tracks TTree will be saved.

## Geometry
The same coordinate system as defined and used in [tgc_analysis](https://gitlab.cern.ch/McGill-sTGC/tgc_analysis) is used in `cosmics_xray_correlation.` The geometry parameters are stored in XML files. All units are in millimeters unless otherwise specified.

## Usage

First, a custom clustering algorithm was used to fit the PDO distribution of muon hits on each layer, stored in the directory, ReClustering. The input to ReClustering is a list of CosmicsAnalysis.root files from which the tracks TTrees are combined and all events therein processed. 

ReClustering usage:

./ReClustering [-o outdirectory] [--tag prefix] [--dnlconfig dnlconfigfile] CosmicsAnalysis.root [CosmicsAnalysis2.root...]

The default output directory is ./out. The prefix is prepended to any output file names ReClustering produces (default none). The --dnlconfig flag will be discussed momentarily. The input is a CosmicsAnalysis.root file, which is generated by [CosmicsAnalysis](https://gitlab.cern.ch/McGill-sTGC/tgc_analysis/-/tree/master/CosmicsAnalysis) . The data from separate CosmicsAnalysis files are analyzed in sequence.

CosmicsAnalysis.root files with tracks trees for each Canadian quadruplet are stored in lxplus.cern.ch:/eos/atlas/atlascerngroupdisk/det-nsw-stgc/cosmics_test_bench_data/QUADTYPE/QUADNAME/ANALYSIS_3100V/CosmicsAnalysis.root.

The user may decide to apply a differential non linearity correction to the recalculated cluster positions, and the amplitude of those DNL corrections is specified in a DNL configuration text file.

The DNL correction takes the form:
$$ \delta*y_{dnl} = a_{m}*sin(2*\pi*y_{rel})$$
as defined in [eqn. 5.8 of Benoit Lefebvre's thesis](https://cds.cern.ch/record/2633639?ln=en).

If no DNL configuration is provided, the default is not to apply a DNL correction. The DNL configuration file has the format:
[cluster multiplicity] [amplitude in millimeters], 
where multiplicity is the number of strips with hits above threshold or neighbour triggered on a particular layer for a particular event. If you want a DNL correction to be applied to all multiplicities or a default amplitude value for clusters with multiplicities without a custom amplitude defined, include an amplitude entry for multiplicity zero as the first line of the configuration file.

The output root file of ReClustering is the input of cosmics_xray_correlation.

cosmics_xray_correlation usage:

Usage: ./CosmicsXRayCorrelation input_reclustering.root QUADNAME input_reclustering.root QUADNAME input_xray_database.db outpath/ tag_

input_reclustering.root is the output of ReClustering. ReClustering files for each Canadian quadruplet are stored in lxplus.cern.ch:/eos/atlas/atlascerngroupdisk/det-nsw-stgc/cosmics_test_bench_data/QUADTYPE/QUADNAME/ANALYSIS_3100V/reclustering/prefix_reclustering.root.
QUADNAME specifies the quadruplet module, with format e.g. QL2P06.
input_xray_database.db is the x-ray database. The most recent x-ray database as of 2021-11-26 is stored in the subdirectory, xrayData.
outpath specifies the outpath directory.
tag_ specifies the prefix with which to name the output files.

## Output

Of ReClustering:
prefix_reclustering.root: Contains reclustering tracks tree and all plots generated in ReClustering.cpp
prefix_reclustering_plots.pdf: Contains prints of plots generated in ReClustering.cpp, mostly distributions of quantities of interest for all clusters on all layers.
prefix_sample_cluster_fit.csv: Information about first 50 cluster fits (see L447-455 of ReClustering.cpp)
ReClustering output is stored in lxplus.cern.ch:/eos/atlas/atlascerngroupdisk/det-nsw-stgc/cosmics_test_bench_data/QUADTYPE/QUADNAME/ANALYSIS_3100V/reclustering

Of cosmics_xray_correlation:
(Note that residuals referes to cosmic muon track residuals unless otherwise specified)
prefix_cosmics_xray_correlation.root: Output file containing all plots generated by CosmicsXRayCorrelation.cpp (grep plot name in src subdirectory to determine exact plot contents)
*_y_evaluation_uncertainties.pdf: Histograms of uncertainty in reconstructed y-track positions for each tracking combination. Only entries in a single bin because uncertainties are the same per tracking combination.
*_residual_uncertainty_hists.pdf: Histograms of uncertainty in residuals for each tracking combination. Only entries in a single bin because uncertainties are the same per tracking combination.
*_track_y_angle_hists.pdf: Histograms of the reconstructed track angles in the y-direction for each tracking combination.
*_residual_distributions.pdf: Histograms of all residuals tabulated using each tracking combination.
*_residual_vs_yrel.pdf: 2D histogram of residual values versus y_{rel} to assess differential non-linearity effects (appendix C.4 of [Lia's thesis](https://github.com/liaformenti/thesis)). First page has residuals calculated using all tracking combinations, and subsequent pages are per tracking combination.
*_residual_vs_yrel_profile.pdf: Profile of 2D histograms of residuals versus y_{rel}. Profile defined in ResPlots::MakeProfileX. See Abusleme 2016 test beam paper for qualitative description of this type of plot.
*_num_entries_\[binning_name\].pdf: The number of residuals calculated with the indicated tracking combination in each region of interest over the surface of a quadruplet. The binning name is a string describing the distribution of the regions of interest, defined in Binning::Binning.
*_residual_fits_\[binning_name\].pdf: The residual distributions in each region of interest (boundaries defined in Binning::Binning) fit with a Gaussian.
*_fit_means_\[binning_name\].pdf: 2D histograms of the Gaussian means of the residual distributions created in each region of interest over the surface of the quadruplet. The residual distributions means are of equal magnitude and opposite sign to the physical relative offset of the strip pattern in that region of interest.
*_fit_sigmas_\[binning_name\].pdf: 2D histograms of the Gaussian sigmas of the residual distributions created in each region of interest over the surface of the quadruplet.
QUADNAME_xray_positions.pdf: Area plot showing the nominal x-ray gun positions.
QUADNAME_xray_data_offsets.txt: List of x-ray offsets calculated for each gas volume where data is available.
QUADNAME_xray_residuals.txt: List of x-ray residuals calculated.
*_cosmic_residuals_in_ROIs.pdf: Cosmic residual distributions in regions of interest around each x-ray point, fit with a Gaussian.
*_local_mean_cosmics_residual_vs_xray_residual_scatter.pdf: Scatter plot of x-ray residual and mean cosmics residuals in the region around the x-ray residual.
`cosmics_xray_correlation` output is stored in lxplus.cern.ch:/eos/atlas/atlascerngroupdisk/det-nsw-stgc/cosmics_test_bench_data/QUADTYPE/QUADNAME/ANALYSIS_3100V/reclustering 

# Analysis notes
If you're considering rerunning the analysis, consider these notes.
