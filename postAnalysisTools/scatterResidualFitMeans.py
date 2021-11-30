# Takes in two analysis output.root files that you want to compare, and
# makes a scatter plot comparing the means for both.
# User specifies which tracking combination (fixed tracking layers and layer the residual is calculate on) they are interested in.

# Lia Formenti
# McGill University
# Feb 17, 2021

import ROOT
import numpy as np
import sys
import atlasplots as aplt

# Create scatter plots and store them in a ROOT file and a pdf
def compareResidualFitMeans(file1Name, file2Name, layer, fixedLayer1, fixedLayer2):

    # If a bin has value -100, it means the fit failed / too few entries in bin
    invalidBinMarker = -100

    # Open files
    file1 = ROOT.TFile(file1Name)
    file2 = ROOT.TFile(file2Name)

    # Get list of all key names in analysis ouput files
    keyNames1 = [key.GetName() for key in file1.GetListOfKeys()]
    keyNames2 = [key.GetName() for key in file2.GetListOfKeys()]

    # Define the trait of the key you want
    # Which plot: 2D histogram of residual means for the user's input tracking combination
    desiredNameEnd = "means_layer" + str(layer) + "_fixedlayers" + str(fixedLayer1)
    desiredNameEnd += str(fixedLayer2)
    sliceFromIndex = len(desiredNameEnd)

    # Loop through keys to find the one you want
    # Get the associated means TH2F
    for name in keyNames1:
        if name[-sliceFromIndex:] == desiredNameEnd:
            print(len(name), name, name[-sliceFromIndex:])
            meansHist1 = file1.Get(name)
            break;
    for name in keyNames2:
        if name[-sliceFromIndex:] == desiredNameEnd:
            meansHist2 = file2.Get(name)
            break;

    # Check that the two histograms are compatible: they must have the same binning to compare
    # residual means in equivalent regions of interest
    if (meansHist1.GetNbinsX() != meansHist2.GetNbinsX() or 
            meansHist1.GetNbinsY() != meansHist2.GetNbinsY()):
        print("The two histograms do not have the same number of bins. Check inputs.")

    # Create a list of means stored in histograms, ignoring those where one of the fits failed
    means1 = []
    means2 = []
    for i in range(1, meansHist1.GetNbinsX() + 1):
        for j in range(1, meansHist1.GetNbinsY() + 1):
            mean1 = meansHist1.GetBinContent(i,j)
            mean2 = meansHist2.GetBinContent(i,j)
            if (mean1 == -100 or mean2 == -100):
                continue
            means1.append(mean1)
            means2.append(mean2)
    # Create scatter plot 
    means1Arr = np.array(means1)
    means2Arr = np.array(means2)
    scatterPlot = ROOT.TGraph(len(means1Arr), means2Arr, means1Arr)
    scatterPlot.GetXaxis().SetTitle("Mean of residuals, method 1 [mm]")
    scatterPlot.GetYaxis().SetTitle("Mean of residuals, method 2 [mm]")
    scatterPlot.Fit("pol1")
    fitFcn = scatterPlot.GetFunction("pol1")
    fitFcn.SetParName(0, "b")
    fitFcn.SetParName(1, "m")
    ROOT.gStyle.SetOptFit(111)

    # Create canvas
    c = ROOT.TCanvas("c","c")
    scatterPlot.Draw("AP")

    # Print scatter plot to file
    c.Print("compareMeans.pdf")
    c.Print("compareMeans.root")

    return;

# Get files to analyze from cmd line
if len(sys.argv) < 6:
    print("Usage: python compareResidualFitMeans.py file1_strip_position_analysis.root " +
          "file2_strip_position_analysis.root layer fixedlayer1 fixedlayer2")
    exit();

# Set style
aplt.set_atlas_style()

# Call function to create scatter plots
compareResidualFitMeans(sys.argv[1], sys.argv[2], sys.argv[3], sys.argv[4], sys.argv[5])


