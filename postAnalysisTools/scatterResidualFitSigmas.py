# Takes in two analysis output root files that you want to compare, and
# compares the sigmas of residual distribution fits in the same region of interest for the same tracking combination. Gets sigmas from the sigma TH2F ResPlots objects stored in the .root file.
# User specifies which tracking combination they are interested in.
# Not sure what to do about outliers

# Lia Formenti
# McGill University
# Feb 17, 2021

import ROOT
import numpy as np
import sys
import atlasplots as aplt

# Function to create scatter plot of residual distribution fit sigmas for equivalent regions of 
# interest for the given tracking combination. Outputs scatter plot to pdf and root file.
def compareResidualFitSigmas(file1Name, file2Name, layer, fixedLayer1, fixedLayer2):

    # If a bin has value -100, it means the fit failed / too few entries in bin
    invalidBinMarker = -100

    # Open files
    file1 = ROOT.TFile(file1Name)
    file2 = ROOT.TFile(file2Name)

    # Get list of key names
    keyNames1 = [key.GetName() for key in file1.GetListOfKeys()]
    keyNames2 = [key.GetName() for key in file2.GetListOfKeys()]

    # Define the trait of the key you want
    # Which plot: sigmas TH2F, which combination: provided by user
    desiredNameEnd = "sigmas_layer" + str(layer) + "_fixedlayers" + str(fixedLayer1)
    desiredNameEnd += str(fixedLayer2)
    sliceFromIndex = len(desiredNameEnd)

    # Loop through keys to find the one you want
    # Get the associated sigmas TH2F
    for name in keyNames1:
        if name[-sliceFromIndex:] == desiredNameEnd:
            print(len(name), name, name[-sliceFromIndex:])
            sigmasHist1 = file1.Get(name)
            break;
    for name in keyNames2:
        if name[-sliceFromIndex:] == desiredNameEnd:
            sigmasHist2 = file2.Get(name)
            break;

    # Check that the two histograms have the same area binning so sigmas in the same area bins
    # can be compared directly in the scatter plot
    if (sigmasHist1.GetNbinsX() != sigmasHist2.GetNbinsX() or 
            sigmasHist1.GetNbinsY() != sigmasHist2.GetNbinsY()):
        print("The two histograms do not have the same number of bins. Check inputs.")

    # Create a list of sigmas stored in histograms, ignoring those where one of the fits failed
    sigmas1 = []
    sigmas2 = []
    for i in range(1, sigmasHist1.GetNbinsX() + 1):
        for j in range(1, sigmasHist1.GetNbinsY() + 1):
            sigma1 = sigmasHist1.GetBinContent(i,j)
            sigma2 = sigmasHist2.GetBinContent(i,j)
            if (sigma1 == -100 or sigma2 == -100):
                continue
            sigmas1.append(sigma1)
            sigmas2.append(sigma2)

    # Create scatter plot 
    sigmas1Arr = np.array(sigmas1)
    sigmas2Arr = np.array(sigmas2)
    scatterPlot = ROOT.TGraph(len(sigmas1Arr), sigmas2Arr, sigmas1Arr)
    scatterPlot.GetXaxis().SetTitle("Sigma of residuals, method 1 [mm]")
    scatterPlot.GetYaxis().SetTitle("Sigma of residuals, method 2 [mm]")
    scatterPlot.Fit("pol1")
    fitFcn = scatterPlot.GetFunction("pol1")
    fitFcn.SetParName(0, "b")
    fitFcn.SetParName(1, "m")
    ROOT.gStyle.SetOptFit(111)

    # Create canvas
    c = ROOT.TCanvas("c","c")
    scatterPlot.Draw("AP")

    # Write out scatter plot to file
    c.Print("compareSigmas.pdf")
    c.Print("compareSigmas.root")

    return;

# Get files to analyze from cmd line
if len(sys.argv) < 6:
    print("Usage: python compareResidualFitsigmas.py file1_strip_position_analysis.root " +
          "file2_strip_position_analysis.root layer fixedlayer1 fixedlayer2")
    exit();

# Set style 
aplt.set_atlas_style()

# Call function to create scatter plot
compareResidualFitSigmas(sys.argv[1], sys.argv[2], sys.argv[3], sys.argv[4], sys.argv[5])



