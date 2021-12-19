# To extract a residual distribution in an area bin for use in a figure.
# Like figure 5.5 in Lia's thesis.
# On line 14 the cosmics_xray_correlation root file to extract from is specified 
# On line 17 the residual distribution to extract is specified. Bins are numbered as described in the ROOT TH1 documentation: bin 0 is underflow, bins 1-n are the area bins, starting from low x (y) to high x (y) and bin n+1 is the overflow bin
import ROOT

c = ROOT.TCanvas("c","c",600,600)
c.SetTopMargin(0.15)
c.SetRightMargin(0.15)
c.SetLeftMargin(0.15)
c.SetBottomMargin(0.16)

ROOT.gStyle.SetOptTitle(0)
ROOT.gStyle.SetErrorX(0.)

f = ROOT.TFile("QL2P11_3100V_2021-08-05_strip_position_analysis.root")

h = f.Get("xbin_width_100mm_ybin_width_100mm_residuals_xbin_12_ybin_7_layer4_fixedlayers13")
h.Draw()
h.GetXaxis().SetTitle("Residual [mm]")
h.SetAxisRange(-3,3,"X")
h.SetTitle("#splitline{Layer : 4, Fixed layers: 1, 3}{x #in [197, 297] mm, y #in [594.6, 694.6] mm}")

titleBox = ROOT.TPaveText(0.05,0.9,0.7,1,"NDC")
titleText = titleBox.AddText(h.GetTitle())
titleText.SetTextSize(0.025)
titleBox.SetFillStyle(0)
titleBox.SetBorderSize(0)
titleBox.Draw()

