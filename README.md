# cosmics\_xray\_correlation

Analysis tools to compare cosmic muon data with [x-ray wedge alignment data](https://twiki.cern.ch/twiki/bin/viewauth/Atlas/XRayWedgeAlignment).

Main author: Lia Formenti, liaformenti\[at\]gmail.com  
Responsible: Brigitte Vachon, brigitte.vachon\[at\]cern.ch  
Last updated: 2021-12-19

## Background

For a complete background, see [Lia Formenti's MSc thesis](https://github.com/liaformenti/thesis).

Small strip thin gap chambers (sTGCs) were constructed for the ATLAS Experiment's New Small Wheels (NSWs). sTGCs are gas ionization chambers designed to detect muons. Four sTGCs are stacked into a quadruplet. Each layer of the quadruplet records information about the passage of a muon. Offline, the recorded data is processed with [`tgc_analysis`](https://gitlab.cern.ch/McGill-sTGC/tgc\_analysis) to reconstruct muon tracks and tabulate other metrics used to characterize the performance of the quadruplet.

The position of the muon in the precision coordinate is recorded by strip electrodes on each layer, which have 3.2 mm pitch. During construction, it is possible that strip layers were shifted off of their nominal position. `cosmics_xray_correlation` uses track residuals to characterize the relative local offset of the strip pattern in regions of interest with respect to two reference layers. 

Offsets of the strip pattern cause a muon's recorded position to be shifted opposite to its real position. With cosmics data, there is no external reference frame to provide the real position, so only information about relative strip position offsets can be extracted. The reference frames used in `cosmics_xray_correlation` use the recorded muon positions on two fixed layers. Cosmic muon tracks are built using the hits on two fixed layers, and residuals are calculated on a third layer. For tracks falling in a region of interest, the mean of residuals on the third layer is calculated. The mean of residuals will be shifted systematically depending on the local relative offset of the strips on the layer of interest with respect to the local pattern of the strips on the two fixed layers, which in turn depends on the absolute misalignment of layers. The set of reference layers used to calculate the residual on the layer of interest is referred to as "the tracking combination."

The [x-ray method](https://twiki.cern.ch/twiki/bin/viewauth/Atlas/XRayWedgeAlignment) tabulates the local offset of the strip pattern at a limited number of locations over the surface of a quadruplet. The complete x-ray production run data is stored in directory, xrayData, in an SQL database, `xray\_results\_2021-11-26.db`. As of 2021-12-19, Stergios Tsigaridas (stergios.tsigaridas\[at\]cern.ch) and Isabel Trigger (isabel.trigger@\[at\]cern.ch) are responsible for the x-ray data.  

X-ray offsets (_ymeas\_raw\_w - y\_corrangle_ as labelled in the database) are used to create an "x-ray track." The offsets are used as the "positions" on each layer used to build a track. X-ray residuals are calculated using a track built from the offsets on two fixed layers. The x-ray residuals can then be compared to the mean of cosmics residuals around them,tabulated with the same reference layers. The program repeats this process for all positions on the quadruplet where x-ray data was taken for all possible sets of reference layers. Several outputs characterizing the cosmics data and comparing the cosmics and x-ray data are generated.

## Getting cosmics\_xray\_correlation

```
git clone --recurse-submodules ssh://git@gitlab.cern.ch:7999/McGill-sTGC/cosmicsxray_correlation.git
```
Alternatively, you may clone using https protocol. The flag --recurse-submodules is required since `cosmics_xray_correlation` takes [`tgc_analysis`](https://gitlab.cern.ch/McGill-sTGC/tgc\_analysis) as a submodule.

## Requirements

`cosmics_xray_correlation` and ReClustering (a program in the so-named subdirectory, described later) have the same dependencies as [`tgc_analysis`](https://gitlab.cern.ch/McGill-sTGC/tgc\_analysis), detailed in its README. To setup the appropriate environment on LXPLUS, run the following:

```
export ATLASLOCAL\_ROOT\_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase
source ${ATLASLOCAL\_ROOT\_BASE}/user/atlasLocalSetup.sh
localSetupROOT 6.14.04-x8664-slc6-gcc62-opt
export ALRBrootVersion=6.14.04-x86\_64-slc6-gcc62-opt
```

Other subdirectories (contents described later) contain various useful Python scripts. All Python code was written for `python3`.

## Compilation of C++ code

All modules and submodules have their own Makefile. So to compile, simply type "make".

## Data preparation

Raw cosmic muon data collected with [`sTGCreadout_sw-master`](https://gitlab.cern.ch/ssun/sTGC\_readout\_sw) must be [decoded](https://gitlab.cern.ch/McGill-sTGC/tgc\_analysis/-/tree/master/vmm3decoder/Decoder) then run through [`CosmicsAnalysis`](https://gitlab.cern.ch/McGill-sTGC/tgc\_analysis/-/tree/master/CosmicsAnalysis) with the saveTracks option set to "on" so that a tracks TTree will be saved. `CosmicsAnalysis` groups contiguous hits on strips into objects called clusters. It fits those clusters with a Gaussian to extract the cluster centroid and records them in the tracks TTree by layer and per event. 

The tracks TTree from a CosmicsAnalysis.root file is extracted. The tracks TTree contains information about every event recorded by the sTGC quadruplet, specifically that which is necessary to build the muon tracks. The most important branches of that tree are:
- posCH: A map containing the position of each strip channels that fired during an event on each layer. The keys of the map specify the layer.
- pdostrip: A map containing the PDO recorded on each strip channel that fired during an event on each layer. The keys of the map specify the layer.
- trackX: The x-position assigned to the recorded charge distribution during `CosmicsAnalysis`.
- trackYGaussian: The original Gaussian mean of the charge distribution recorded on the strips during `CosmicsAnalysis`.


## Geometry
The same coordinate system as defined and used in [`tgc_analysis`](https://gitlab.cern.ch/McGill-sTGC/tgc\_analysis) is used in `cosmics_xray_correlation.` The geometry parameters are stored in XML files. All units are in millimeters unless otherwise specified.

## Usage

First, a custom clustering algorithm was used to fit the PDO distribution of recorded hits (the clusters) on each layer, stored in the directory, ReClustering. The input to `ReClustering` is a list of CosmicsAnalysis.root files from which the tracks TTrees are combined and all events therein processed. 

### `ReClustering` usage:

./ReClustering [-o outdirectory] [--tag prefix] [--dnlconfig dnlconfigfile] CosmicsAnalysis.root [CosmicsAnalysis2.root...]

The default output directory is ./out. The prefix is prepended to any output file names `ReClustering` produces (default none). The --dnlconfig flag will be discussed momentarily. The input is a CosmicsAnalysis.root file, which is generated by [`CosmicsAnalysis`](https://gitlab.cern.ch/McGill-sTGC/tgc_analysis/-/tree/master/CosmicsAnalysis) . The data from separate `CosmicsAnalysis` files are analyzed in sequence.

CosmicsAnalysis.root files with tracks trees for each Canadian quadruplet are stored in lxplus.cern.ch:/eos/atlas/atlascerngroupdisk/det-nsw-stgc/cosmicstest\_bench\_data/QUADTYPE/QUADNAME/ANALYSIS\_3100V/CosmicsAnalysis.root.

The user may decide to apply a differential non linearity correction to the recalculated cluster positions, and the amplitude of those DNL corrections is specified in a DNL configuration text file.

The DNL correction takes the form:
$$ \delta*y{dnl} = a\_{m}*sin(2*\pi*y\_{rel})$$
as defined in [eqn. 5.8 of Benoit Lefebvre's thesis](https://cds.cern.ch/record/2633639?ln=en).

If no DNL configuration is provided, the default is not to apply a DNL correction. The DNL configuration file has the format:
[cluster multiplicity] [amplitude in millimeters], 
where multiplicity is the number of strips with hits above threshold or neighbour triggered on a particular layer for a particular event. If you want a DNL correction to be applied to all multiplicities or a default amplitude value for clusters with multiplicities without a custom amplitude defined, include an amplitude entry for multiplicity zero as the first line of the configuration file.

The output root file of `ReClustering` is the input of `cosmics_xray_correlation`.

### `cosmics_xray_correlation` usage:

Usage: ./CosmicsXRayCorrelation inputreclustering.root QUADNAME input\_reclustering.root QUADNAME input\_xray\_database.db outpath/ tag\_

input\_reclustering.root is the output of `ReClustering`. `ReClustering` files for each Canadian quadruplet are stored in lxplus.cern.ch:/eos/atlas/atlascerngroupdisk/det-nsw-stgc/cosmics\_test\_bench\_data/QUADTYPE/QUADNAME/ANALYSIS\_3100V/reclustering/prefix\_reclustering.root.
QUADNAME specifies the quadruplet module, with format e.g. QL2P06.
inputxray\_database.db is the x-ray database. The most recent x-ray database as of 2021-11-26 is stored in the subdirectory, xrayData.
outpath specifies the outpath directory.
tag specifies the prefix with which to name the output files.

## Output

### Of `ReClustering`:
- prefix\_reclustering.root: Contains `ReClustering` tracks tree and all plots generated in ReClustering.cpp  
- prefix\_reclustering\_plots.pdf: Contains prints of plots generated in ReClustering.cpp, mostly distributions of quantities of interest for all clusters on all layers.  
- prefix\_sample\_cluster\_fit.csv: Information about first 50 cluster fits (see L447-455 of ReClustering.cpp)  

ReClustering output is stored in lxplus.cern.ch:/eos/atlas/atlascerngroupdisk/det-nsw-stgc/cosmicstest\_bench\_data/QUADTYPE/QUADNAME/ANALYSIS\_3100V/reclustering


### Of `cosmics_xray_correlation`:  
(Note that residuals referes to cosmic muon track residuals unless otherwise specified)  
- tag\_cosmics\_xray\_correlation.root: Output file containing all plots generated by CosmicsXRayCorrelation.cpp (grep plot name in src subdirectory to determine exact plot contents)  
- \*y\_evaluation\_uncertainties.pdf: Histograms of uncertainty in reconstructed y-track positions for each tracking combination. Only entries in a single bin because uncertainties are the same per tracking combination.  
- \*residual\_uncertainty\_hists.pdf: Histograms of uncertainty in residuals for each tracking combination. Only entries in a single bin because uncertainties are the same per tracking combination.  
- \*track\_y\_angle\_hists.pdf: Histograms of the reconstructed track angles in the y-direction for each tracking combination  
- \*residual\_distributions.pdf: Histograms of all residuals tabulated using each tracking combination.  
- \*residual\_vs\_yrel.pdf: 2D histogram of residual values versus y\_{rel} to assess differential non-linearity effects (appendix C.4 of [Lia's thesis](https://github.com/liaformenti/thesis)). First page has residuals calculated using all tracking combinations, and subsequent pages are per tracking combination.  
- \*residual\_vs\_yrel\_profile.pdf: Profile of 2D histograms of residuals versus y\_{rel}. Profile defined in ResPlots::MakeProfileX. See Abusleme 2016 test beam paper for qualitative description of this type of plot.  
- \*num\_entries\_\[binning\_name\].pdf: The number of residuals calculated with the indicated tracking combination in each region of interest over the surface of a quadruplet. The binning name is a string describing the distribution of the regions of interest, defined in Binning::Binning.  
- \*residual\_fits\_\[binning\_name\].pdf: The residual distributions in each region of interest (boundaries defined in Binning::Binning) fit with a Gaussian.  
- \*fit\_means\_\[binning\_name\].pdf: 2D histograms of the Gaussian means of the residual distributions created in each region of interest over the surface of the quadruplet. The residual distributions means are of equal magnitude and opposite sign to the physical relative offset of the strip pattern in that region of interest.  
- \*fit\_sigmas\_\[binning\_name\].pdf: 2D histograms of the Gaussian sigmas of the residual distributions created in each region of interest over the surface of the quadruplet.  
- QUADNAME\_xray\_positions.pdf: Area plot showing the nominal x-ray gun positions.  
- QUADNAME\_xray\_data\_offsets.txt: List of x-ray offsets calculated for each gas volume where data is available.  
- QUADNAME\_xray\_residuals.txt: List of x-ray residuals calculated.  
- \*cosmic\_residuals\_in\_ROIs.pdf: Cosmic residual distributions in regions of interest around each x-ray point, fit with a Gaussian  
- \*local\_mean\_cosmics\_residual\_vs\_xray\_residual\_scatter.pdf: Scatter plot of x-ray residual and mean cosmics residuals in the region around the x-ray residual.  
- \*local\_cosmic\_and\_xray\_data.csv: Contains the information stored on the scatter plot.
  
`cosmics_xray_correlation` output is stored in lxplus.cern.ch:/eos/atlas/atlascerngroupdisk/det-nsw-stgc/cosmics\_test\_bench\_data/QUADTYPE/QUADNAME/ANALYSIS\_3100V/cosmics\_xray\_correlation.

# Other subdirectories

Some useful Python scripts are contained in some of the subdirectories of this module. The description of what each file does is in the .py file itself. These scripts are less robust than the main body of the code in the src and include subdirectories.

- postAnalysisTools: Some analysis methods to be run on the output of `cosmics_xray_correlation`. `compareResidualFits.py` compares the cosmics residual means calculated over the surface of the quadruplet for each tracking combination between two runs of cosmics x-ray correlation. `aggregateAnalysis.py` and `regroupingCorrelationPlots.py` do some analyses of correlation between mean cosmics and x-ray residuals of all quadruplets instead of by quadruplet.
- plottingCode: Scripts to generate figures used in [Lia's thesis](https://github.com/liaformenti/thesis). Often, the changes are stylistic.
- usefulScripts: As the name suggests. Includes a minimal script called testGeom.cpp to manipulate the tgc\_analysis geometry object and some sample code on how to generate a sql database from a csv file.


# Analysis notes
If you're considering rerunning the analysis, consider these notes. They may include useful information about the code structure so that you can find what you need to change.

## Notes about `ReClustering`
The main body of the `ReClustering` algorithm is in ReClustering.cpp. 

`ReClustering` does the cluster again by fitting a Gaussian (using a different algorithm than `CosmicsAnalysis` uses) to posCH and pdostrip. See appendix A of [Lia's thesis](https://github.com/liaformenti/thesis). It stores the fit parameters of new Gaussian fit in branches named with the prefix "r\_".

Some of the strips on the long edge of a quadruplet are really noisy. A warning will be printed to stdout if this is the case (see L482 of ReClustering.cpp). If required to run `ReClustering` again, for the quads listed startingat L328 of ReClustering.cpp, uncomment L335-349 and adjust accordingingly to reject all clusters originally recorded at that top strip.

## Notes about `cosmics_xray_correlation`  
The main body of this program is in CosmicsXRayCorrelation.cpp. There are four main steps of this analysis, called from the main:  
1. Get the x-ray data from xrayresults\_2021-11-26.db and store them in XRayData objects (L97).  
2. Create tracks from the x-ray offsets and calculate the x-ray residuals. The x-ray residuals are stored in the residuals member vector of the XRayRetracking class.  
3. Retrack the cosmics data. Loop over every event stored in the `ReClustering` tracks TTree and rebuild the track for all possible sets of reference layers. Calculate residuals for all possible tracking combinations and store them in a residuals vector (of Residual objects).  
4. Compare the cosmics and x-ray data. For each x-ray residual, define a region of interest around it (with widths in x and y specified in the constructor). Fit the residual distribution for all residuals of the given tracking combination falling in the region of interest with a Gaussian. Store the x-ray residual and mean cosmics residuals in a LocalData object. The CompareData class keeps a vector of the LocalData objects. Put each x-ray and mean cosmics residual pair in a scatter plot to assess the correlation. Output the LocalData object parameters to csv.  
5. Create various plots from the cosmics data only using the ResPlots class. Most important are the 2D histograms of residual means for each tracking combination over the surface of the quadruplet. These are visual indications of the relative misalignments between layers of the quadruplet.

The x-ray data is extracted from the database using a sqlite3 select statement defined on L32 of XRayData.cpp. Edit the select statement to change what x-ray data gets selected. The quad the x-ray measurement was taken on is not included in the x-ray database. To select data from a paticular quadruplet, the select statement asks for entries on the correct wedge (appropriate MTF for the quad) and of the correct quad type. The entries returned are ordered by xnom to sort them by x-ray gun position.

In the language of the column names in the sql database, the x-ray offset = ymeas\_raw\_w - y\_corrangle. The uncertainty in the x-ray offset is 120 um (L93), as accepted by the working group.

The tracks in the input tracks tree are re-tracked with two reference layers in CosmicsRetracking.cpp. The uncertainty on the cluster is 60 um (assigned on L54) based on appendix A in [Lia's thesis](https://github.com/liaformenti/thesis). 

There is no real reason to use the `ReClustering` tracks tree over the CosmicsAnalysis tracks tree anymore; however `cosmics_xray_correlation` reads from the tracks TTree branches and uses the names of the branches in the `ReClustering` tracks TTree. If you would like to edit the script again to use the `CosmicsAnalysis` tracks TTree, you should just have the change the branch names accessed in CosmicsRetracking.cpp.

You can change the size of the region of interest to define around x-ray gun positions by sending in a different width in x and y to the CompareData object constructor.

The cosmics residual mean 2D histograms are made using the class ResPlots. ResPlots creates a series of 2D and 1D histograms from the CosmicsRetracking residuals vector using the same Binning object. The Binning object defines the regions of interest over the surface of the quadruplet by storing the bin edges in a vector. Binning objects also have a name to describe how the binning is defined. The Binning class is written such that you can define a constructor to tabulate the bin edges you want. The one used currently (L132 of CosmicsXRayCorrelation.cpp) tabulates event bins over the surface of the quad based on the user's desired bin widths in x and y respectively. If you want a new Binning, write a new Binning constructor that tabulates your desired bin edges.

ResPlots is structured so that each plot type has an Initialize, Create and Print method. The Initialize method add the plots to the PlotManager object. The Create method is designed to be called by the user. The Create method calls the initialize method and fills the initialized plots. The Print method outputs the plots to PDF.
