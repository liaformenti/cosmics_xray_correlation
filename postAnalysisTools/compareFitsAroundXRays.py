# This script compares the cosmic residual means and sigmas in regions of interest 
# around the x-ray points.
# It takes in two analysis files, retrieved the residual distributions in all regions of interest (around all gun positions for all possible sets of two fixed layers and a layer of interest). 
# It takes the difference of the means and sigmas of corresponding fits (same gun position, fixed layers, and layer of interest) and puts those differences into distributions.
# Outputs a PDF with difference in means distribution, difference in corresponding sigmas distribution, and sigma distributions.
# Ideal for seeing the effect of different analysis choices on the residual distributions.
# Only comparing fits around x-ray points allows you to compare the size of the region of interest
# between runs.

import ROOT
import numpy as np
import sys
import atlasplots as aplt
import uncertainties as u

def compareFitsAroundXRays(file1Name, file2Name, legendText1, legendText2):

    # Get analysis input files
    file1 = ROOT.TFile(file1Name)
    file2 = ROOT.TFile(file2Name)

    # Select out the residual distributions around each x-ray gun position and
    # for each set of two fixed layers and layer of interest
    # Store the names of all corresponding plots in an array
    # The plots of interest all have names that start with "cosmic_residuals_around_xray_point_"
    keyNames1 = [key.GetName() for key in file1.GetListOfKeys()]
    keyNames2 = [key.GetName() for key in file2.GetListOfKeys()]
    keyNamePrefix = "cosmic_residuals_around_xray_point_"
    prefixLength = len(keyNamePrefix)
    keyNames = []
    for keyName in keyNames1:
        if keyName[:prefixLength] == keyNamePrefix:
            if keyName in keyNames2:
                keyNames.append(keyName)

    # Select only interpolation/extrapolation combinations
    #     interpStrs = ["layer2_fixedlayers13",
    #                   "layer2_fixedlayers14",
    #                   "layer3_fixedlayers14",
    #                   "layer3_fixedlayers24"]
    #     extrapStrs=["layer3_fixedlayers12",
    #                 "layer4_fixedlayers12",
    #                 "layer4_fixedlayers13",
    #                 "layer1_fixedlayers23",
    #                 "layer4_fixedlayers23",
    #                 "layer1_fixedlayers24",
    #                 "layer1_fixedlayers34",
    #                 "layer2_fixedlayers34"]
    # 
    #     print(len(keyNames))
    #     for keyName in keyNames:
    #         comboStr = keyName[len(keyName)-comboStrLen:]
    #         print(keyName, comboStr)
    #         if comboStr not in extrapStrs:
    #             keyNames.remove(keyName)
    #             print("removed")
    #         else: 
    #             print("keep")
    # 

    # Initialize the plots
    meanDiffs = ROOT.TH1F("mean_diffs", ";Difference in residual mean [mm]; No. entries", 32, -0.5, 0.5)
    sigmaDiffs = ROOT.TH1F("sigma_diffs", ";Difference in residual #sigma [mm];No. entries", 32, -0.5, 0.5)
    sigmaDist1 = ROOT.TH1F("sigma_distribution_1", ";Residual distribution #sigma [mm];No. entries", 32,-0.5,0.5)
    sigmaDist2 = ROOT.TH1F("sigma_distribution_2", ";Residual distribution #sigma [mm];No. entries", 32,-0.5,0.5)

    for name in keyNames:
        # Get the corresponding residual distribution for each analysis file
        h1 = file1.Get(name)
        h2 = file2.Get(name)
        # Get the fits
        f1 = h1.GetFunction("myGaus")
        f2 = h2.GetFunction("myGaus")
        if f1!=None and f2!=None: # If both fits succeeded
            # Get the fit parameters
            mean1 = f1.GetParameter(1)
            sigma1 = f1.GetParameter(2)
            mean2 = f2.GetParameter(1)
            sigma2 = f2.GetParameter(2)
            # Fill the plots
            meanDiffs.Fill(mean2-mean1)
            sigmaDist1.Fill(sigma1)
            sigmaDist2.Fill(sigma2)
            sigmaDiffs.Fill(sigma2-sigma1)
        elif f1==None:
            if f2!=None:
                sigma2 = f2.GetParameter(2)
                sigmasDist2.Fill(sigma2)
        elif f2==None:
            sigma1 = f1.GetParameter(2)
            sigmasDist1.Fill(sigma1)

    # Prep outfile file name
    # prefix + (prefix of input file 2) minus (prefix of input file 1)
    # Get rid of directory path in filenames
    file1Name = file1Name.split("/")[-1]
    file2Name = file2Name.split("/")[-1]
    fileNameSuffix = "_strip_position_analysis.root"
    sliceOffLength = len(fileNameSuffix) 
    outFileName = "compare_residual_fits_around_xrays_" + file2Name[:len(file2Name)-sliceOffLength] + "_minus_"
    outFileName += file1Name[:len(file1Name)-sliceOffLength] 

    # Create canvas
    c = ROOT.TCanvas("c","c")
    # Write plots to output pdf
    c.Print(outFileName + ".pdf[")
    # Difference in residual distribution fitted means distribution
    ROOT.gStyle.SetOptStat(2210)
    ROOT.gROOT.ForceStyle()
    meanDiffs.Draw()
    # Put the RMS on the plot
    mean = u.ufloat(meanDiffs.GetMean(), meanDiffs.GetMeanError())
    rms = u.ufloat(meanDiffs.GetRMS(), meanDiffs.GetRMSError())
    print()
    print("Mean: ", mean)
    print("RMS: ", rms)
    print()
    meanStr = "{}".format(mean).split('+')[0]
    rmsStr  = "{}".format(rms).split('+')[0]
    textBox = ROOT.TPaveText(0.2,0.75,0.5,0.9,"NDC")
    firstLine = textBox.AddText("Mean: " + meanStr  + " mm")
    secondLine = textBox.AddText("RMS: " + rmsStr + " mm")
    firstLine.SetTextSize(30)
    secondLine.SetTextSize(30)
    textBox.SetFillStyle(0)
    textBox.SetBorderSize(0)
    textBox.Draw()
    c.Print(outFileName + ".pdf")
    c.SaveAs(outFileName + "_means_difference.png")
    c.Clear()
    # Difference in residual distribution fitted sigmas distribution
    ROOT.gStyle.SetOptStat(0)
    ROOT.gROOT.ForceStyle()
    sigmaDist1.SetLineColor(9) # blue
    sigmaDist1.SetMarkerStyle(10) # none
    sigmaDist1.SetTitle(legendText1)
    sigmaDist1.Draw()
    sigmaDist2.SetLineColor(416+3) # forest green
    sigmaDist2.SetMarkerStyle(10) # none
    sigmaDist2.SetTitle(legendText2)
    sigmaDist2.Draw("same")
    c.BuildLegend()
    c.Print(outFileName + ".pdf")
    c.Clear()
    # Distribution of sigmas fof each file
    ROOT.gStyle.SetOptStat(2210)
    ROOT.gROOT.ForceStyle()
    sigmaDiffs.Draw()
    c.Print(outFileName + ".pdf")
    c.Print(outFileName + ".pdf]")

    # Output to root file
    outFile = ROOT.TFile(outFileName + ".root", "RECREATE")
    meanDiffs.Write("difference_in_residual_means")
    sigmaDiffs.Write("difference_in_residual_sigmas")
    sigmaDist1.Write("sigma_dist_1_" + legendText1.replace(" ", "_"))
    sigmaDist2.Write("sigma_dist_2_" + legendText2.replace(" ", "_"))

    return

### MAIN ###
# Get files to analyze from cmd line
# legendText is a string (entered on the command line enclosed in single quotes) that will
# be used to describe the input file or the difference between the two files you are comparing.
# E.g. if you're comparing analysis of 2900V data vs 3100V data, labels '2900 V' and '3100 V' 
# may be appropriate.
if len(sys.argv) < 5:
    print("Usage: python compareResidualFits.py file1_strip_position_analysis.root " +
          "file2_strip_position_analysis.root legendText1 legendText2")
    exit()

# Set ATLAS style
aplt.set_atlas_style()
style = ROOT.gROOT.GetStyle("ATLAS")
# Call comparison / plot making function
compareFitsAroundXRays(sys.argv[1], sys.argv[2], sys.argv[3], sys.argv[4])


