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

# Takes in a list of analysis output files to analyze. 
# Produces the distribution of the slope, offset and chi2 parameters from each correlation plot and prints them to a pdf and a root file.
# Also writes the slope, offset and chi2 of each input file to a csv

def aggregateCorrelationParameters(inFileNames, tag):

    # Define histograms to hold slope, offset, chi2 and respective uncertainties
    slopeHist = ROOT.TH1F("slopeHist", ";Slope;No. entries", 20, 0, 2)
    slopeUncertHist = ROOT.TH1F("slopeUncertHist", ";Slope fit uncertainty;No. entries", 10, 0, 0.1)
    offsetHist = ROOT.TH1F("offsetHist", ";Offset [mm];No. entries", 20, -0.05, 0.05)
    offsetUncertHist = ROOT.TH1F("offsetUncertHist", "Offset fit uncertainty [mm];No. entries",10, 0, 0.01)
    chi2OverNDFHist = ROOT.TH1F("chi2Hist", ";#chi^{2};No. entries", 10, 0, 100)

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

### MAIN ###
aplt.set_atlas_style()
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
theTag = "aggregate_results_" + datetime.today().strftime('%Y-%m-%d')
aggregateCorrelationParameters(inFileList, theTag)

