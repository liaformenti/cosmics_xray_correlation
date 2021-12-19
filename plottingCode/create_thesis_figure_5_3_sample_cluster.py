# Create a plot of the QL2C04 2900V event 5 layer 2 cluster
# Run as python -i create_thesis_figure_5_4_fake_event_display.py so that canvas pops up in GUI
# Save figure from there
import ROOT
import numpy as np
import atlasplots as aplt

aplt.set_atlas_style()

# Cluster data
ids = [118, 119, 120, 121, 122] # strip id
positions = [387, 390.2, 393.4, 396.6, 399.8] # mm
pdos = [39, 303, 617, 232, 34] # ADC units
amp = 617.34 # ADC units
mean_mm = 393.149
sigma_mm = 2.49309
mean = 119.922 # in "strip id" units
sigma = 0.779091 # in "strip id" units

c = ROOT.TCanvas("c","c",600,600)

# Plot the cluster and its fit
# Cluster graph
g = ROOT.TGraph(len(ids))
# g.SetFillColorAlpha(38, 0.8) # blue
g.SetFillStyle(0)
g.SetLineColor(1)
# g.SetLineWidth(2)
# g.SetLineColor(1)
# g.SetLineSize(3)
for j in range(g.GetN()):
    g.SetPoint(j, positions[j], pdos[j])

# Customize x axis
# Range is half strip extra
gMin = positions[0]-1.6
gMax = positions[len(positions)-1]+1.6

# Cluster fit function
f = ROOT.TF1("clusterfit", "gaus", gMin, gMax)
f.SetParameter(0, amp)
f.SetParameter(1, mean_mm)
f.SetParameter(2, sigma_mm)
f.SetLineColor(16)
f.SetLineStyle(9)

# Copy axes from graph (automatic and correct) to fit function
f.GetYaxis().SetLimits(g.GetYaxis().GetXmin(), g.GetYaxis().GetXmax())
f.SetTitle(";Strip position [mm];Peak signal amplitude (arbitrary units)")
# Remove labels from y axis
yAxis = f.GetYaxis()
for lab in range(yAxis.GetNdivisions()):
    yAxis.ChangeLabel(lab, -1, 0, -1, -1, -1, "")

f.Draw()
print(ROOT.gStyle.GetBarWidth())
desiredWidth = 1.21 # approx 3.2 mm / (2.5 mm per default bar width)
ROOT.gStyle.SetBarWidth(desiredWidth)
g.Draw("B")
