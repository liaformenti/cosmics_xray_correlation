# Overlay the overall residual distribution from two analysis files for residuals calculated from tracks built from clusters on two fixed layers and polated to the layer of interest.
import ROOT
import numpy as np
import sys
import atlasplots as aplt

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

# Overlays residual distributions for each tracking combination and outputs them to a pdf
# Takes in the names of two analysis files and text to to use to distinguish them in the legend
def overlayResidualFits(file1Name, file2Name, legendText1, legendText2):
 
    # Open analysis files
    file1 = ROOT.TFile(file1Name)
    file2 = ROOT.TFile(file2Name)

    # Create canvas
    c = ROOT.TCanvas("c", "c")

    # Create output file for plot pdfs
    file1Name = file1Name.split("/")[-1]
    file2Name = file2Name.split("/")[-1]
    fileNameSuffix = "_strip_position_analysis.root"
    sliceOffLength = len(fileNameSuffix)
    # Output file name is prefix + (colour assigned to first input file) + 
    # (first input file's prefix) + (colour assigned to second input file) +
    # (second input file's prefix)
    outFileName = "residual_distributions_blue_" + file1Name[:len(file1Name)-sliceOffLength] + "_green_" + file2Name[:len(file2Name)-sliceOffLength]
    c.Print(outFileName + ".pdf[")

    # Get list of key names for all plots in analysis files
    keyNames1 = [key.GetName() for key in file1.GetListOfKeys()]
    keyNames2 = [key.GetName() for key in file2.GetListOfKeys()]

    # For each tracking combination
    combos = GetComboList()
    for combo in combos:
        # Select out the residual distribution by the name of the plot
        # To select the correct plot names,
        # define the trait of the key you want
        # Which plot: residual distribution for specific combination
        desiredNameEnd = "residual_distribution_layer" + str(combo.layer) + "_fixedlayers" 
        desiredNameEnd += str(combo.la) + str(combo.lb)
        sliceFromIndex = len(desiredNameEnd)
    
        # Get the desired residual distribution
        for name in keyNames1:
            if name[-sliceFromIndex:] == desiredNameEnd:
                resDist1 = file1.Get(name)
                break;
        for name in keyNames2:
            if name[-sliceFromIndex:] == desiredNameEnd:
                resDist2 = file2.Get(name)
                break;

        # Normalize the residual distributions so they can be plotted over one another
        # for better comparison of their shapes
        # resDist1.Sumw2(ROOT.kTRUE)
        # resDist2.Sumw2(ROOT.kTRUE)
        resDist1.Scale(1/resDist1.Integral())
        resDist2.Scale(1/resDist2.Integral())

        # Draw and style
        resDist1.SetMarkerColor(9) # blue
        resDist1.SetMarkerStyle(20) # circle
        resDist1.SetLineColor(10) # white
        resDist1.SetTitle(legendText1)
        resDist1.GetYaxis().SetTitle("Normalized entries")
        # gausFit1 = ROOT.TF1("gausFit1", "gaus", -10, 10)
        # gausFit1.SetLineColor(9) # blue
        # resDist1.Fit("gausFit1")
        resDist1.Draw()

        resDist2.SetMarkerColor(416+3) # forest green
        resDist2.SetMarkerStyle(22) # triangle 
        resDist2.SetLineColor(10) # white
        resDist2.SetTitle(legendText2)
        # gausFit2 = ROOT.TF1("gausFit2", "gaus", -10, 10)
        # gausFit2.SetLineColor(416+3) # forest green 
        # resDist2.Fit("gausFit2")
        resDist2.Draw("same")
        # fitBox = ROOT.TPaveText(0.7,0.7,0.9,0.9,"nbNDC")
        # line1 = fitBox.AddText("#mu: " + str(resDist1.GetFunction("gaus").GetParameter(1)))
        # line1.SetTextColor(9) # blue
        # line1.SetTextSize(15)
        # line2 = fitBox.AddText("#sigma: " + str(resDist1.GetFunction("gaus").GetParameter(2)))
        # line2.SetTextColor(9)
        # line2.SetTextSize(15)
        # line3 = fitBox.AddText("#mu : " + str(resDist2.GetFunction("gaus").GetParameter(1)))
        # line3.SetTextColor(416+3)
        # line3.SetTextSize(15)
        # line4 = fitBox.AddText("#mu : " + str(resDist2.GetFunction("gaus").GetParameter(2)))
        # line4.SetTextColor(416+3)
        # line3.SetTextSize(15)
        # fitBox.Draw()
        
        # Create legend
        legend = c.BuildLegend(0.2,0.7,0.4,0.9,"Layer: " + str(combo.layer) + ", fixed layers: " + str(combo.la) + ", " + str(combo.lb))
        legend.SetTextSize(15)
        # c.SetTitle("Layer: " + str(combo.layer) + ", fixed layers: " + str(combo.la) + ", " + str(combo.lb))
        # Print overlaid plots to pdf
        c.Print(outFileName + ".pdf")
        c.Clear()

    c.Print(outFileName + ".pdf]")
    return

# Get files to analyze from cmd line
if len(sys.argv) < 5:
    print("Usage: python overlayResidualDistributions.py file1_strip_position_analysis.root " +
          "file2_strip_position_analysis.root legendText1 legendText2")
    exit()

aplt.set_atlas_style()
overlayResidualFits(sys.argv[1], sys.argv[2], sys.argv[3], sys.argv[4])


