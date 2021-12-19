# Check out Lia Formenti's MSc thesis, figure 5.4, to see what this code outputs.
# Run as python -i create_thesis_figure_5_4_fake_event_display.py so that canvas pops up in GUI
# Save figure from there
# It's a fake event display because all four layer's plots use the same y-axis, but in reality the strips are staggered between layers.
# The purpose of this figure is to get the idea of the effect of misalignments on the residuals of a cosmics track.

import ROOT
import numpy as np
import atlasplots as aplt

aplt.set_atlas_style()

# Event 5, QL2C04_2900V.
# Different entries correspond to difference layers

amps = [130.794, 617.34, 458.831, 254.676] # PDO counts
means_mm = [388.85, 393.149, 396.559, 401.085]
sigmas_mm = [2.26493, 2.49309, 2.04683, 2.59274]
means = [119.078, 119.922, 121.487, 122.402] # strip ID
sigmas = [0.707789, 0.779091, 0.639634, 0.810231] # change in strip ID
ids = [ [118, 119, 120], 
        [118, 119, 120, 121, 122],
        [120, 121, 122, 123],
        [121, 122, 123, 124]
      ]
positions = [ [385.4, 388.6, 391.8], # mm
              [387, 390.2, 393.4, 396.6, 399.8],
              [391.8, 395, 398.2, 401.4],
              [396.6, 399.8, 403, 406.2]
            ]
pdos = [ [41, 130, 56],
         [39, 303, 617, 232, 34],
         [28, 344, 332, 31],
         [56, 226, 193, 38]
       ]

# Shift the layer 2 cluster over by two strip IDs to amplify the effect of a misalignment on the
# cluster position
shift_strips = 2 
pitch = 3.2 # mm
for i in range(len(ids[1])):
    ids[1][i] += shift_strips
    positions[1][i] += shift_strips*pitch # shift over by two strip pitches
means[1] += shift_strips # shift over by two strips

c = ROOT.TCanvas("c", "c")#, 600, 800) 
c.cd()
c.Draw()
# c.Divide(1,4)
# See May 10 diagram in notes
# User coordinates unless otherwise specified (whole canvas, range is 0 to 1)
# Margin in ROOT is % space between pad boundary and frame around plot. Used oddly here.
# Pad length in user coordinates includes root margins!
nPads = 4
botPadExtra = 1.5 # Extra length added in no. of yMargins to bottom pad for x axis label
yMargin = 0.05 # space between pads, so true margin in user coords is half
yPad = (1 - botPadExtra*yMargin)/4
yMarginPercent = (yMargin/2)/yPad
xPadMargin = yMargin*2 # where pad starts in x
xPadExtraLeftMargin = yMargin*2.5
xPadLeftMargin = xPadMargin + xPadExtraLeftMargin
xMargin = 0.01
xPad = 1-xPadMargin-xPadLeftMargin # width of pad in x in user coordinates
# Create pads
p1 = ROOT.TPad("p1", "p1", xPadMargin, botPadExtra*yMargin + 3*yPad, 1-xPadLeftMargin, 1, -1, -1, -2)
p1.SetTopMargin(yMarginPercent)
p1.SetBottomMargin(yMarginPercent)
p1.SetFillStyle(4000)
p1.Draw()
p2 = ROOT.TPad("p2", "p2", xPadMargin, botPadExtra*yMargin + 2*yPad, 1-xPadLeftMargin, botPadExtra*yMargin + 3*yPad, -1, -1, -2)
p2.SetTopMargin(yMarginPercent)
p2.SetBottomMargin(yMarginPercent)
p2.SetFillStyle(4000)
p2.Draw()
p3 = ROOT.TPad("p3", "p3", xPadMargin, botPadExtra*yMargin + yPad, 1-xPadLeftMargin, botPadExtra*yMargin + 2*yPad, -1, -1, -2)
p3.SetTopMargin(yMarginPercent)
p3.SetBottomMargin(yMarginPercent)
p3.SetFillStyle(4000)
p3.Draw()
p4 = ROOT.TPad("p4", "p4", xPadMargin, 0, 1-xPadLeftMargin, botPadExtra*yMargin + yPad, -1, -1, -2)
p4.SetTopMargin((yMargin/2)/(yPad + botPadExtra*yMargin))
p4.SetBottomMargin(2*yMargin/(yPad + botPadExtra*yMargin))
p4.SetFillStyle(4000)
p4.Draw()
pads = []
pads.append(p1)
pads.append(p2)
pads.append(p3)
pads.append(p4)
c.Update()
# Create pad "legend" text (gas gap labels)
# For beside pads
# t1 = ROOT.TLatex(1-xPadLeftMargin, botPadExtra*yMargin + 3.5*yPad, "#splitline{Layer 1}{(Reference)}")
# t1.SetTextAlign(12)
# t2 = ROOT.TLatex(1-xPadLeftMargin, botPadExtra*yMargin + 2.5*yPad, "#bf{Layer 2}")
# t2.SetTextAlign(12)
# t3 = ROOT.TLatex(1-xPadLeftMargin, botPadExtra*yMargin + 1.5*yPad, "Layer 3")
# t3.SetTextAlign(12)
# t4 = ROOT.TLatex(1-xPadLeftMargin, botPadExtra*yMargin + 0.5*yPad, "#splitline{Layer 4}{(Reference)}")
# t4.SetTextAlign(12)
# # Need list to decide color in loop since color should match fit line color
# gasGapLabels = []
# gasGapLabels.append(t1)
# gasGapLabels.append(t2)
# gasGapLabels.append(t3)
# gasGapLabels.append(t4)

clusterGraphs = []
clusterFits = []
padGapLabels = []
labelText = ["#splitline{Layer 1}{(Reference)}", "#bf{Layer 2}", "Layer 3", "#splitline{Layer 4}{(Reference)}"]

for i in range(4): # For each layer
     
    # Get params
    curID = np.array(ids[i])
    curPDO = np.array(pdos[i])
    amp = amps[i]
    mean = means[i]
    sigma = sigmas[i]
    print(amp, mean, sigma)
    if len(curID) != len(curPDO):
        print("Length of ID and PDO lists not the same")
        exit(1)

    # Choose pad
    pads[i].cd()
    # pads[i].SetFillStyle(4000)

    # Set x-margins
    ROOT.gPad.SetRightMargin(xMargin)
    ROOT.gPad.SetLeftMargin(xMargin)

    # Create cluster fit
    fMin = mean - len(curID)/2 - 0.5
    fMax = mean + len(curID)/2 + 0.5
    # f = ROOT.TF1("clusterfit", "gaus", 117, 125.5)
    f = ROOT.TF1("clusterfit", "gaus", fMin, fMax)
    f.SetParameter(0, amps[i])
    f.SetParameter(1, means[i])
    f.SetParameter(2, sigmas[i])

    # Pad gap label
    print(labelText[i])
    # if i==1:
    #     # c.cd()
    #     # padGapLabel = ROOT.TLatex(0.95, 1-0.175*yPad, labelText[i])
    #     # pads[i].cd()
    # elif i !=3:
    if i != 3:
        padGapLabel = ROOT.TLatex(0.95, 0.825, labelText[i])
    else: # to account for different pad size for gap 4
        # Brute force this positioning you're so confused.
        padGapLabel = ROOT.TLatex(0.95, 0.875, labelText[i])
    padGapLabel.SetNDC() # Necessary for text to draw correctly on pad!
    padGapLabel.SetTextSize(30)
    padGapLabel.SetTextAlign(33)

    # Color by layer
    if i==0 or i==3:
        f.SetLineColor(632+1) # soft red
        # gasGapLabels[i].SetTextColor(632+1)
        padGapLabel.SetTextColor(632+1)
    elif i==1:
        f.SetLineColor(416+3) # forest green
        # gasGapLabels[i].SetTextColor(416+3)
        padGapLabel.SetTextColor(416+3)
    elif i==2:
        f.SetLineColor(38) # soft blue
        f.SetLineStyle(2) # dash
        # gasGapLabels[i].SetTextColor(38)
        padGapLabel.SetTextColor(38)

    # Create cluster graph
    g = ROOT.TGraph(len(curID))
    g.SetFillColor(38) # soft blue
    for j in range(g.GetN()):
        g.SetPoint(j, curID[j], curPDO[j])
    # g.Draw("Bsame")
    # g.GetXaxis().SetLimits(117, 125.5)
    g.GetHistogram().SetMaximum(650)
    g.GetHistogram().SetMinimum(0)
    yAxis = g.GetYaxis();
    numLabels = yAxis.GetNdivisions()
    # Get rid of the yaxis labels
    for lab in range(numLabels):
       yAxis.ChangeLabel(lab, -1, 0, -1, -1, -1, "")

    # X-axis
    xAxis = g.GetXaxis();
    xAxis.SetLimits(117, 125.5)
    # xAxis.SetLimits(117, 125.5)
    # Only keep the x axis labels for the gap 4 plot
    if i != 3:
        numLabels = xAxis.GetNdivisions()
        for lab in range(numLabels):
            xAxis.ChangeLabel(lab, -1, 0, -1, -1, -1, "")
    else: # For the gap 4 axis,
        xAxis.SetLabelSize(30)
        xAxis.SetTitleSize(30)
        xAxis.SetTitle("Strip ID")
        xAxis.SetTitleOffset(3.5)
        xAxis.ChangeLabel(0, -1, 0, -1, -1, -1, "") # Get rid of first label, which is cut off
    g.Draw("AB")
    f.Draw("same")
    padGapLabel.Draw()
    ROOT.gPad.Modified()
    ROOT.gPad.Update()

    # Make objects persistent
    clusterGraphs.append(g)
    clusterFits.append(f)
    padGapLabels.append(padGapLabel)

# cTest = ROOT.TCanvas("ctest", "ctest")
# cTest.cd()
# cTest.Divide(1,2)
# cTest.cd(1)
# ROOT.gPad.SetFillStyle(4000)
# ROOT.gPad.Draw()
# cTest.cd(2)
# ROOT.gPad.SetFillStyle(4000)
# ROOT.gPad.Draw()
# hist = ROOT.TH1F("h","h",10,-1,1)
# hist.FillRandom("gaus")
# hist.Draw()
# pave = ROOT.TPaveText(0.5,0.5,0.8,0.8,"trNDC")
# pave.AddText(0.2,0.2,"test")
# pave.Draw()
# padGapLabels[1].Draw()
# ROOT.gPad.Modified()
# ROOT.gPad.Update()
# print(padGapLabels[1])
# 
c.cd()
# y label
yLabel = ROOT.TText(0.075, 0.97, "Signal Amplitude (arbitrary units)")
yLabel.SetTextAlign(32)
yLabel.SetTextSize(30)
yLabel.SetTextAngle(90)
yLabel.SetTextColor(1)
yLabel.Draw()
# We draw a transparent pad over all the other pads to put the annotations
# Creation of transparent pad
linePad = ROOT.TPad("linePad", "linePad", 0, 0, 1, 1, -1, -1, 2)
linePad.SetFillStyle(4000)
linePad.Draw()
c.Update()
linePad.cd()
# Creation of muon track line
xAxisToUserCoords = (xPad - xMargin) / (125.5-117) # user coords / axis coords
yAxisToUserCoords = (yPad - yMargin)/650
x1 = xPadMargin + xMargin*xPad + (means[0] - 117)*xAxisToUserCoords
y1 = 1 - yMargin/2 - (650-clusterFits[0].Eval(means[0]))*yAxisToUserCoords
x2 = xPadMargin + xMargin*xPad + (means[3] - 117)*xAxisToUserCoords
y2 = botPadExtra*yMargin + 0.5*yMargin + clusterFits[3].Eval(means[3])*yAxisToUserCoords
m = (y2-y1)/(x2-x1)
b = y2 - m*x2
print(m,b)
# Extend the track a little bit past the means
x1 -= 0.025
x2 += 0.025
y1 = m*x1 + b
y2 = m*x2 + b
line = ROOT.TArrow(x1, y1, x2, y2, 0.02, ">")
line.SetLineColor(632+1) # soft red
line.SetFillColor(632+1)
line.SetLineStyle(1)
line.SetLineWidth(3)
line.Draw()
# Muon symbol
muon = ROOT.TLatex(x1-0.005, y1+0.005, "#mu")
muon.SetTextSize(30)
muon.SetTextAlign(31)
muon.Draw()
# Residual arrow
# arrY2 is Y position of layer 2 cluster mean
arrY2 = botPadExtra*yMargin + 2*yPad + 0.5*yMargin + (clusterFits[1].Eval(means[1])*yAxisToUserCoords)
arrY1 = arrY2
arrX2 = xPadMargin + xMargin*xPad + (means[1]-117)*xAxisToUserCoords
arrX1 = (arrY1 - b)/m
arrow = ROOT.TArrow(arrX1, arrY1, arrX2, arrY2, 0.02, "<||>")
arrow.SetLineColor(416+3)
arrow.SetFillColor(416+3)
arrow.SetLineWidth(3)
arrow.Draw()
# Residual label
xResLabel = arrX1 + (arrX2 - arrX1)/2
# yResLabel = arrY1 - 0.02
yResLabel = arrY1 + 0.02
resLabel = ROOT.TLatex(xResLabel, yResLabel, "#bf{Residual}")
resLabel.SetTextAlign(21)
resLabel.SetTextSize(30)
resLabel.SetTextColor(416+3)
resLabel.Draw()
# Draw gas gap labels
# for label in gasGapLabels:
#   label.Draw()
