# This script compares the cosmic residual means and sigmas fitted in regions of interest over the area of the quadruplet for residuals calculated from tracks built from hits on two given fixed layers and polated to the given layer of interest. 
# It takes in two analysis files, and retrieves the residual distributions in all regions of interest for the given set of two fixed layers and layer of interest. 
# It takes the difference of the means and sigmas of corresponding fits (same region of interest) and puts those differences into distributions.
# Outputs a PDF with difference in means distribution, difference in corresponding sigmas distribution, and sigma distributions.
# Ideal for seeing the effect of different analysis choices on the residual distributions.
# !! The two input analysis files must have been tabuated with the same binning over the area of the quad (same width and height of the region of interest in x and y). !! Otherwise there is no way to compare corresponding residual distributions.

# Lia Formenti
# McGill University
# Feb 17, 2021

import ROOT
import numpy as np
import sys
# style
import atlasplots as aplt
import uncertainties as u

def compareResidualFits(file1Name, file2Name, layer, fixedLayer1, fixedLayer2, legendText1, legendText2):

    # If a region of interest bin has value -100, it means the fit failed / too few entries in bin
    invalidBinMarker = -100

    # Open files
    file1 = ROOT.TFile(file1Name)
    file2 = ROOT.TFile(file2Name)

    # Get list of names of residual distribution plots
    keyNames1 = [key.GetName() for key in file1.GetListOfKeys()]
    keyNames2 = [key.GetName() for key in file2.GetListOfKeys()]

    ## DIFFERENCE IN RESIDUAL MEANS ##

    # Define the trait of the key you want
    # Which plot: means TH2F, which combination: provided by user
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
            print(len(name), name, name[-sliceFromIndex:])
            meansHist2 = file2.Get(name)
            break;

    # Check that the two histograms are compatible
    if (meansHist1.GetNbinsX() != meansHist2.GetNbinsX() or 
            meansHist1.GetNbinsY() != meansHist2.GetNbinsY()):
        print("The two histograms do not have the same number of bins. Check inputs.")
        return;

    # Create histogram to store difference using copy constructor
    meanDiffs = ROOT.TH1F("compare_means", ";Difference in residual means [mm];No. entries", 32, -0.5, 0.5)
    # For not including edge area bins in mean diffs distribution
    # for i in range(4, meansHist1.GetNbinsX() - 4 + 1):
    #    for j in range(2, meansHist1.GetNbinsY()): #+ 1):
    for i in range(1, meansHist1.GetNbinsX() + 1):
        for j in range(1, meansHist1.GetNbinsY() + 1):
            mean1 = meansHist1.GetBinContent(i,j)
            mean2 = meansHist2.GetBinContent(i,j)
            if (mean1 == -100 or mean2 == -100):
                continue
            else:
                meanDiffs.Fill(mean2-mean1)

    ## DISTRIBUTION AND DIFFERENCE IN RESIDUAL SIGMAS ## 

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

    sigmaDist1 = ROOT.TH1F("sigma_distribution_1", ";Residual distribution #sigma [mm]; No. entries", 32, 0, 3);
    sigmaDist2 = ROOT.TH1F("sigma_distribution_2", ";Residual distribution #sigma [mm]; No. entries", 32, 0, 3);
    sigmaDiffs = ROOT.TH1F("compare_sigmas", ";Difference in residual #sigma [mm];No. entries", 32, -0.5, 0.5)
    for i in range(1, sigmasHist1.GetNbinsX() + 1):
        for j in range(1, sigmasHist1.GetNbinsY() + 1):
            sigma1 = sigmasHist1.GetBinContent(i,j)
            sigma2 = sigmasHist2.GetBinContent(i,j)
            if (sigma1 == -100 or sigma2 == -100):
                continue
            else:
                sigmaDist1.Fill(sigma1)
                sigmaDist2.Fill(sigma2)
                sigmaDiffs.Fill(sigma2-sigma1)

    # Fit
    # meanDiffs.Fit("gaus")
    # sigmaDiffs.Fit("gaus")
    # Prep outfile file name
    # Get rid of directory path in filenames
    file1Name = file1Name.split("/")[-1]
    file2Name = file2Name.split("/")[-1]
    fileNameSuffix = "strip_position_analysis.root"
    sliceOffLength = len(fileNameSuffix) 
    outFileName = "compare_residual_fits_" + file2Name[:len(file2Name)-sliceOffLength] + "minus_"
    outFileName += file1Name[:len(file1Name)-sliceOffLength] + "layer" + str(layer) + "_fixedlayers"
    outFileName += str(fixedLayer1) + str(fixedLayer2)
    
    # Get stat box
    # ROOT.gStyle.SetOptStat("eMR")
    # Create canvas
    c = ROOT.TCanvas("c","c")
    # Write plots to output
    c.Print(outFileName + ".pdf[")
    # Mean diffs
    # ROOT.gStyle.SetOptStat(2210)
    # ROOT.gROOT.ForceStyle()
    meanDiffs.Draw()
    # Put the RMS on the plot
    mean = u.ufloat(meanDiffs.GetMean(), meanDiffs.GetMeanError())
    rms = u.ufloat(meanDiffs.GetRMS(), meanDiffs.GetRMSError())
    print()
    print("Mean: ", mean)
    print("RMS: ", rms)
    print()
    meanStr = "{:.2f}".format(mean).split('+')[0]
    rmsStr  = "{:.2f}".format(rms).split('+')[0]
    textBox = ROOT.TPaveText(0.2,0.75,0.5,0.9,"NDC")
    firstLine = textBox.AddText("Mean: " + meanStr  + " mm")
    secondLine = textBox.AddText("RMS: " + rmsStr + " mm")
    firstLine.SetTextSize(30)
    secondLine.SetTextSize(30)
    textBox.SetFillStyle(0)
    textBox.SetBorderSize(0)
    textBox.Draw()

    c.Print(outFileName + ".pdf")
    # c.SaveAs(outFileName + "mean_difference.png")
    c.Clear()
    # Sigma distributions
    # ROOT.gStyle.SetOptStat(0)
    # ROOT.gROOT.ForceStyle()
    sigmaDist1.SetLineColor(9) # blue
    sigmaDist1.SetMarkerStyle(10) # none
    sigmaDist1.SetTitle(legendText1)
    sigmaDist1.Draw()
    sigmaDist2.SetLineColor(416+3) # forest green
    sigmaDist2.SetMarkerStyle(10) # none
    sigmaDist2.SetTitle(legendText2)
    sigmaDist2.Draw("same")
    print(sigmaDist1.GetMean(), sigmaDist2.GetMean())
    # info = ROOT.TPaveText(0.2,0.7,0.55,0.9,"NDC")
    # firstLine = info.AddText("               Mean")
    # firstLine.SetTextSize(30)
    # firstLine.SetTextAlign(12)
    # secondLine = info.AddText("3100 V    0.27 mm")
    # secondLine.SetTextColor(9) # blue
    # secondLine.SetTextSize(30)
    # secondLine.SetTextAlign(12)
    # thirdLine = info.AddText("2900 V    0.34 mm")
    # thirdLine.SetTextColor(416+3) # forest green
    # thirdLine.SetTextSize(30)
    # thirdLine.SetTextAlign(12)
    # info.Draw()
    c.BuildLegend()
    c.Print(outFileName + ".pdf")
    c.Clear()
    # Sigma diffs
    ROOT.gStyle.SetOptStat(2210)
    ROOT.gROOT.ForceStyle()
    sigmaDiffs.Draw()
    c.Print(outFileName + ".pdf")
    # c.SaveAs(outFileName + "sigma_difference.png")
    c.Print(outFileName + ".pdf]")
    outFile = ROOT.TFile(outFileName + ".root", "RECREATE")
    meanDiffs.Write("difference_in_residual_means")
    sigmaDiffs.Write("difference_in_residual_sigmas")
    return

### MAIN ###
# Get files to analyze from cmd line
if len(sys.argv) < 8:
    print("Usage: python compareResidualFits.py file1_strip_position_analysis.root " +
          "file2_strip_position_analysis.root layer fixedlayer1 fixedlayer2 legendText1 legendText2")
    exit()

# Set ATLAS style
aplt.set_atlas_style()
style = ROOT.gROOT.GetStyle("ATLAS")
# style.SetOptStat("emr")
style.SetOptFit(111)
# Call comparison / plot making function
compareResidualFits(sys.argv[1], sys.argv[2], sys.argv[3], sys.argv[4], sys.argv[5], sys.argv[6], sys.argv[7])


