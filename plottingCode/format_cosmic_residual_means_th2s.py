import ROOT
import atlasplots as aplt
import sys

# A combination is the set of two reference layers used to build a cosmic muon track and the layer the residual is calculated on.
class Combination:
    def __init__(self, layer, fixedlayer1, fixedlayer2):
        if (type(layer) != type(1) or type(fixedlayer1) != type(1) or type(fixedlayer2) != type(1)):
                print("Layer numbers should be entered as integers between 1 and 4.\n")
                exit(1)
        if (layer < 1 or fixedlayer1 < 1 or fixedlayer2 < 1):
            print("Layer numbers should be between 1 and 4.\n")
            exit(1)
        if (layer > 4 or fixedlayer1 > 4 or fixedlayer2 > 4):
            print("Layer numbers should be between 1 and 4.\n")
            exit(1)
        if (layer == fixedlayer1 or layer == fixedlayer2 or fixedlayer1 == fixedlayer2):
            print("All layers in a combination must be unique.\n")

        self.l = layer
        if fixedlayer1 < fixedlayer2:
            self.fl1 = fixedlayer1
            self.fl2 = fixedlayer2
        else:
            self.fl1 = fixedlayer2
            self.fl2 = fixedlayer1

    def comboPrint(self):
        print('Layer:', self.l, 'Fixed layers: ', self.fl1, ',', self.fl2)

# List of tracking combinations to loop through
allCombos = [Combination(3,1,2),
             Combination(4,1,2),
             Combination(2,1,3),
             Combination(4,1,3),
             Combination(2,1,4),
             Combination(3,1,4),
             Combination(1,2,3),
             Combination(4,2,3),
             Combination(1,2,4),
             Combination(3,2,4),
             Combination(1,3,4),
             Combination(2,3,4)]

# Produce a pdf with cosmic residual means TH2s for each tracking combination for input analysis output root file named "filename."
# The colour scale of the plot will range from +/- scaleMax.
def makeAllCombosPDF(filename, scaleMax=0.5):
    # Get file
    f = ROOT.TFile(filename)

    # Build outfile name
    justFileName = filename.split("/")[-1]
    removeSuffix = "strip_position_analysis.root"
    sliceOffLength = len(removeSuffix)
    outFileName = justFileName[:len(justFileName)-sliceOffLength] + "residual_mean_th2s.pdf"
    print(outFileName)
    
    #  Canvas formatting
    c = ROOT.TCanvas("c","c", 1000, 600)
    c.cd()
    c.SetRightMargin(0.15)
    c.SetTopMargin(0.15)
    c.Print(outFileName + "[")

    for combo in allCombos: 
        # Get desired plot
        h = f.Get("xbin_width_100mm_ybin_width_100mm_means_layer" + str(combo.l) + "_fixedlayers" + str(combo.fl1) + str(combo.fl2))
        print(h, scaleMax)

        # Histogram axis
        # h.GetYaxis().SetCanExtend(True)
        # Histogram formatting
        h.SetMaximum(scaleMax)
        h.SetMinimum(-1*scaleMax)
        h.GetZaxis().SetTitle("Mean residual from cosmics [mm]")
        # h.GetZaxis().SetNdivisions(20)
        h.Draw("colz")
        
        histTitle = ROOT.TPaveText(0.1, 0.85, 0.55, 1.0, "NDC")
        histTitle.AddText("Layer : " + str(combo.l) + ", fixed layers: " + str(combo.fl1) + ", " + str(combo.fl2))
        histTitle.SetTextFont(42)
        histTitle.SetTextSize(0.05)
        histTitle.SetBorderSize(0)
        histTitle.SetFillStyle(0)
        histTitle.Draw()

        palette = h.GetListOfFunctions().FindObject("palette")
        palette.SetY2NDC(0.85)
        
        # aplt.atlas_label(text="Work in progress", loc="lower left")
        
        # have to save canvas drawn with xming in full screen by hand to get proportions correct
        c.Print(outFileName)

    c.Print(outFileName + "]")
    f.Close()

    
if len(sys.argv) < 2:
    print("Usage: python format_cosmic_residual_means_th2s.py filename_strip_position_analysis.root [scale maximum in mm]")

aplt.set_atlas_style()
style = ROOT.gROOT.GetStyle("ATLAS")
style.SetOptFit(111)
# style.SetOptTitle(1)
# style.SetTitleSize(0.05, "t")
if (len(sys.argv)==2): # If no scale maximum is given on cmd line, use default of 0.5 mm
    makeAllCombosPDF(sys.argv[1])
else:
    print(sys.argv[1], sys.argv[2])
    makeAllCombosPDF(sys.argv[1], float(sys.argv[2]))
