import ROOT
import numpy as np
import sys
import atlasplots as aplt

# A tracking combination is the set of two fixed layers that were used to the build the track and the layer of interest that the track was polated to to calculate a residual.
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

# Make a list of all possible tracking combinations.
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

# Takes the DNL profiles for a given tracking combination from two input analysis files and overlays
# them onto one plot.
# Outputs the overlay plots for each tracking combination to a PDF in the current directory.
def overlayDNLProfiles(file1Name, file2Name, legendText1, legendText2):
 
    # Open files
    file1 = ROOT.TFile(file1Name)
    file2 = ROOT.TFile(file2Name)

    # Create canvas
    c = ROOT.TCanvas("c", "c", 800, 600)
    c.SetBottomMargin(0.2)
    # Create output file for plot pdfs
    file1Name = file1Name.split("/")[-1]
    file2Name = file2Name.split("/")[-1]
    fileNameSuffix = "_strip_position_analysis.root"
    sliceOffLength = len(fileNameSuffix)
    # Outfile name is prefix + (prefix of first analysis file) + green + (prefix of second analysis file
    outFileName = "dnl_profiles_blue_" + file1Name[:len(file1Name)-sliceOffLength] + "_green_" + file2Name[:len(file2Name)-sliceOffLength]
    c.Print(outFileName + ".pdf[")

    # Get list of all key names of plots in analysis files
    keyNames1 = [key.GetName() for key in file1.GetListOfKeys()]
    keyNames2 = [key.GetName() for key in file2.GetListOfKeys()]

    # For each tracking combination,
    combos = GetComboList()
    for combo in combos:
        # Select out the DNL profiles
        # To select the correct plot names,
        # Define the trait of the key you want
        desiredNameEnd = "residual_vs_yrel_layer" + str(combo.layer) + "_fixedlayers" 
        desiredNameEnd += str(combo.la) + str(combo.lb) + "_profile"
        sliceFromIndex = len(desiredNameEnd)
    
        # Get the desired residual distribution
        for name in keyNames1:
            if name[-sliceFromIndex:] == desiredNameEnd:
                prof1 = file1.Get(name)
                break;
        for name in keyNames2:
            if name[-sliceFromIndex:] == desiredNameEnd:
                prof2 = file2.Get(name)
                break;

        # Draw and style the DNL profiles from each analysis file
        prof1.SetMarkerColor(9) # blue
        prof1.SetMarkerStyle(20) # circle
        prof1.SetTitle(legendText1)

        prof2.SetMarkerColor(416+3) # forest green
        prof2.SetMarkerStyle(22) # triangle 
        prof2.SetTitle(legendText2)

        # Put the profiles from each analysis file into a MultiGraph
        mg = ROOT.TMultiGraph()
        mg.Add(prof1)
        mg.Add(prof2)
        mg.SetTitle(";y_{rel,hit};Mean residual [mm]")
        mg.Draw("ap")

        # Create the legend
        legend = c.BuildLegend(0.6,0.7,0.9,0.9,"Layer: " + str(combo.layer) + ", fixed layers: " + str(combo.la) + ", " + str(combo.lb))
        legend.SetTextSize(15)
        # Figure generation stuff
        # if combo.layer == 4 and combo.la == 1 and combo.lb == 2:
        #    c.SaveAs("picture.png")
        # c.SetTitle("Layer: " + str(combo.layer) + ", fixed layers: " + str(combo.la) + ", " + str(combo.lb))
        # Print to PDF
        c.Print(outFileName + ".pdf")
        c.Clear()

    c.Print(outFileName + ".pdf]")
    return

### MAIN ###
# Get files to analyze from cmd line
# Legend text refers to what text will be the key in the legend corresponding to the DNL profile
# from each file. For example, if you're comparing the effect of data collection voltage on the DNL
# profiles, the legend text might be '2900 V' or '3100 V'
if len(sys.argv) < 5:
    print("Usage: python overlayResidualDistributions.py file1_strip_position_analysis.root " +
          "file2_strip_position_analysis.root legendText1 legendText2")
    exit()

# Set ATLAS style
aplt.set_atlas_style()
# Call plot to overlay the DNL profiles tabulated for each tracking combination.
overlayDNLProfiles(sys.argv[1], sys.argv[2], sys.argv[3], sys.argv[4])


