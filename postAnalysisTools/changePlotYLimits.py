# From an analysis output file, retrieve a plot, change draw it with new limits on the y axis
# and print it to a pdf.

import ROOT

# Get analysis file
f = ROOT.TFile("QL2P08_3100V_2021-06-17_no_dnl_strip_position_analysis.root")
outFileName = "QL2P08_3100V_2021-06-17_residual_vs_yrel_xbin_width_100mm_ybin_width_100mm_new_limits.pdf"

# Create canvas
c = ROOT.TCanvas()
c.SetRightMargin(0.15)

c.Print(outFileName + "[")

# Get plot you want to change limits on
h = f.Get("xbin_width_100mm_ybin_width_100mm_residual_vs_yrel")
# Actually change the limits
h.GetYAxis().SetLimits(-5,5)
h.Draw("colz")
c.Print(outFileName)
c.Clear()
# 
# If the plot you want to change limits on has a version for all tracking combinations,
layers = [3,4,2,4,2,3,1,4,1,3,1,2]
fixedLayers1 = [1,1,1,1,1,1,2,2,2,2,3,3]
fixedLayers2 = [2,2,3,3,4,4,3,3,4,4,4,4]
# 
for i in range(len(layers)):
    name = "xbin_width_100mm_ybin_width_100mm_residual_vs_yrel_layer" + str(layers[i]) + "_fixedlayers" + str(fixedLayers1[i]) + str(fixedLayers2[i])
    print(name)
    h = f.Get(name);
    # Actually change the limits
    h.GetYaxis().SetLimits(-5,5)
    h.Draw("colz")
    c.Print(outFileName)
    c.Clear()
 
c.Print(outFileName + "]")
