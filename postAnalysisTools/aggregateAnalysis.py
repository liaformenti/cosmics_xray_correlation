# Takes in a list of analysis output files to analyze.
# Performs various analyses on all of them designed to explore patterns across all quadruplets of a given geoemtry.
# Each function does a different analysis.
# Lia Formenti
# 2021-12-01

import ROOT
import numpy as np
import atlasplots as aplt
import sys
from datetime import datetime
#
# A tracking combination is a set of two fixed layers used to define a track and the layer that track is polated to to calculate residuals on.
class Combination:
    def __init__(self, layer, fixedLayer1, fixedLayer2):
        if ( (layer < 1 or layer > 4) or 
             (fixedLayer1 < 1 or fixedLayer1 > 4) or 
             (fixedLayer2 < 1 or fixedLayer2 > 4) ):
            raise ValueError("Layer numbers must be between 1 and 4.")

        self.layer = layer

        # Convention is that the smaller numerical layer value (1,2,3 or 4) is the fixed fixed layer
        if fixedLayer1 < fixedLayer2:
            self.la = fixedLayer1
            self.lb = fixedLayer2
        elif fixedLayer2 < fixedLayer1:
            self.la = fixedLayer2
            self.lb = fixedLayer1

        self.string = "layer" + str(self.layer) + "_fixedlayers" + str(self.la) + str(self.lb)

# Defines all tracking combinations
def GetComboList():
    combos = []
    combos.append(Combination(3,1,2))
    combos.append(Combination(4,1,2))
    combos.append(Combination(2,1,3))
    combos.append(Combination(4,1,3))
    combos.append(Combination(2,1,4))
    combos.append(Combination(3,1,4))
    combos.append(Combination(1,2,3))
    combos.append(Combination(4,2,3))
    combos.append(Combination(1,2,4))
    combos.append(Combination(3,2,4))
    combos.append(Combination(1,3,4))
    combos.append(Combination(2,3,4))
    return combos

# Takes in a list of analysis output files to analyze. 
# Produces the distribution of the slope, offset and chi2 parameters from each correlation plot and prints them to a pdf and a root file.
# Also writes the slope, offset and chi2 of each input file to a csv

def aggregateCorrelationParameters(inFileNames, tag):

    # Define histograms to hold slope, offset, chi2 and respective uncertainties
    slopeHist = ROOT.TH1F("slopeHist", ";Slope;No. entries", 20, 0, 2)
    slopeUncertHist = ROOT.TH1F("slopeUncertHist", ";Slope fit uncertainty;No. entries", 10, 0, 0.1)
    offsetHist = ROOT.TH1F("offsetHist", ";Offset [mm];No. entries", 20, -0.05, 0.05)
    offsetUncertHist = ROOT.TH1F("offsetUncertHist", ";Offset fit uncertainty [mm];No. entries",10, 0, 0.01)
    chi2OverNDFHist = ROOT.TH1F("chi2Hist", ";#chi^{2}/ndf;No. entries", 10, 0, 100)

    # Setup csv file
    # Change to cmd line arg out file path
    outFileNameBase = "out/" + tag + "_correlation_parameters"
    csvOutFile = open(outFileNameBase + ".csv", 'w')
    csvOutFile.write("file_name,slope,slope_uncertainty,offset,offset_uncertainty,chi2,ndf\n")

    # For each input file,
    for fileName in inFileNames:
        # Write filename to csv (acts as row key)
        csvOutFile.write(fileName + ',')

        # Open file
        f = ROOT.TFile.Open(fileName)
        # Get overall correlation plot (all tracking combinations)
        scatterPlot = f.Get("local_cosmic_and_xray_residuals_scatter")
        # Get linear fit to correlation plot
        linFit = scatterPlot.GetFunction("pol1")
        slope = linFit.GetParameter(1)
        slopeUncert = linFit.GetParError(1)
        offset = linFit.GetParameter(0)
        offsetUncert = linFit.GetParError(0)
        chi2 = linFit.GetChisquare()
        ndf = linFit.GetNDF()
        print(slope, slopeUncert, offset, offsetUncert, chi2, ndf)
        csvOutFile.write(str(slope) + ',' + str(slopeUncert) + ',' + str(offset) + ',' + str(offsetUncert) + ',' + str(chi2) + ',' + str(ndf) + '\n')
        slopeHist.Fill(slope)
        slopeUncertHist.Fill(slopeUncert)
        offsetHist.Fill(offset)
        offsetUncertHist.Fill(offsetUncert)
        chi2OverNDFHist.Fill(chi2/ndf)

    # Print distributions to pdf
    c = ROOT.TCanvas("c","c",800,600)
    c.Print(outFileNameBase + "_distribution.pdf[")
    slopeHist.Draw()
    c.Print(outFileNameBase + "_distribution.pdf")
    c.Clear()
    offsetHist.Draw()
    c.Print(outFileNameBase + "_distribution.pdf")
    c.Clear()
    chi2OverNDFHist.Draw()
    c.Print(outFileNameBase + "_distribution.pdf")
    c.Clear()
    slopeUncertHist.Draw()
    c.Print(outFileNameBase + "_distribution.pdf")
    c.Clear()
    offsetUncertHist.Draw()
    c.Print(outFileNameBase + "_distribution.pdf")
    c.Clear()
    c.Print(outFileNameBase + "_distribution.pdf]")

    # Write distributions to root file
    rootOutFile = ROOT.TFile(outFileNameBase + "_distributions.root", "RECREATE")
    slopeHist.Write("slope_distribution")
    offsetHist.Write("offset_distribution")
    chi2OverNDFHist.Write("chi2_over_ndf_distribution")
    slopeUncertHist.Write("slope_uncertainty_distribution")
    offsetUncertHist.Write("slope_uncertainty_distribution")
    rootOutFile.Close()

# Fits Gaussian to overall mean distribution (same way as Gaussian fits to residual distributions
# in smaller regions of interest) and makes distribution of means per tracking combination to search for systematic patterns in global offset. Outputs distribution of means in global PDF and values of Gaussian means in csv
def aggregateOverallResidualDistributionMeans(inFileNames, tag):

    combos = GetComboList() 

    # Change to cmd line arg out file path
    outFileNameBase = "out/" + tag + "_overall_residual_means_by_tracking_combination"

    # Setup csv file
    csvOutFile = open(outFileNameBase + ".csv", 'w')
    csvOutFile.write(",")
    # Write csv file header (combinations)
    # And initialize combo's histogram
    resMeanDistsByCombo = {}
    for combo in combos:
        csvOutFile.write(combo.string + ',')
        resMeanDistsByCombo[combo.string] = ROOT.TH1F(combo.string, "", 50, -5, 5)
    csvOutFile.write('\n')

    # Setup canvas
    c = ROOT.TCanvas("c", "c", 800, 600)
    c.Print(outFileNameBase + "_distributions.pdf[")

    # For each input file,
    for fileName in inFileNames:
        # First column of row is filename
        csvOutFile.write(fileName + ",\n")

        inFile = ROOT.TFile(fileName)
        keyNames = [key.GetName() for key in inFile.GetListOfKeys()]

        resDists = []

        # For each tracking combination
        for combo in combos:
            # Select out the residual distribution by the name of the plot
            # To select the correct plot names,
            # define the trait of the key you want
            # Which plot: residual distribution for specific combination
            desiredNameEnd = "residual_distribution_layer" + str(combo.layer) + "_fixedlayers" 
            desiredNameEnd += str(combo.la) + str(combo.lb)
            sliceFromIndex = len(desiredNameEnd)

            # Get the desired residual distribution
            for name in keyNames:
                if name[-sliceFromIndex:] == desiredNameEnd:
                    print(name)
                    resDist = inFile.Get(name)
                    resDists.append(resDist)
                    break;
            # Grab the residual distribution explicitly
            # (maybe not required but just in case there is a scoping issue)
            resDist = resDists[len(resDists)-1]
            resDistMean = resDist.GetMean()
            resDistRMS= resDist.GetRMS()
            print(resDistMean, resDistRMS)
            fit = ROOT.TF1("myGaus", "gaus", resDistMean-resDistRMS, resDistMean+resDistRMS)
            fit.SetParameter(0,100) # Guess for amplitude
            fit.SetParameter(1, resDistMean)
            fit.SetParameter(2, resDistRMS)
            status = resDist.Fit("myGaus", "SQRL")
            if status != 0:
                print("Warning: fit failed for", fileName, combo.string)
                csvOutFile.write(',')
                continue
            else:
                resDistGausMean = fit.GetParameter(1)
                # Fill the distribution of residual distribution means by combination histogram
                resMeanDistsByCombo[combo.string].Fill(resDistGausMean)
                csvOutFile.write(str(resDistGausMean) + ',')
        csvOutFile.write('\n')
            
    
    for comboKey in resMeanDistsByCombo:
        resMeanDistsByCombo[comboKey].Draw()
        labelBox = ROOT.TPaveText(0.2,0.75,0.5,0.9,"NDC")
        labelText = labelBox.AddText(comboKey)
        labelText.SetTextSize(30)
        labelBox.SetFillStyle(0)
        labelBox.SetBorderSize(0)
        labelBox.Draw()
        c.Print(outFileNameBase + "_distributions.pdf")
        print(resMeanDistsByCombo[comboKey].GetMean())

    c.Print(outFileNameBase + "_distributions.pdf]")
    csvOutFile.close()

### MAIN ###
aplt.set_atlas_style()
ROOT.gStyle.SetOptStat(1)
ROOT.gROOT.ForceStyle()
# List of names of analysis output root files to include in analysis
inFileList = []
# Get cmd line args
# Just do skeleton for today, Dec. 1 so you can make some distributions
pos = 1
while pos < len(sys.argv):
    # Deal with input file list in .txt file
    if sys.argv[pos] == "-l":
        fileListFile = open(sys.argv[pos+1])
        for fileName in fileListFile:
            inFileList.append(fileName[:-1])
        pos += 2

# Until you setup a tag flag,
theTag = "QL2C_aggregate_results_" + datetime.today().strftime('%Y-%m-%d')
aggregateOverallResidualDistributionMeans(inFileList, theTag)

