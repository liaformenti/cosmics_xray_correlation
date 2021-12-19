# Takes in the xray_residuals.txt file output from cosmics_xray_correlation for a QL2 formatted as a csv (line 47) and draws the x-ray residuals starting at the nominal gun position on the surface of a quadruplet layer and with length proportional to the x-ray residual for the tracking combination specified on line 50.
# Also draws on the edges of the quadruplet for visualization purposes. To adapt this code for QS3Ps or other geometries, you need to calculate the equation of the line representing the edges of the quadruplet and modify lines 20-39 accordingly.
import ROOT
import atlasplots as aplt
import numpy as np
import pandas as pd

aplt.set_atlas_style()

# Get xray residuals
# combinations holds layer,fixedlayerA,fixedlayerB
# positions holds x,y of residual (bin by this)
# residuals holds the residual, and the residual error
# combinations = np.loadtxt("QL2P08_xray_residuals.txt",dtype='int',delimiter=',',skiprows=1,usecols=[0,1,2])
# positions = np.loadtxt("QL2P08_xray_residuals.txt",dtype='float',delimiter=',',skiprows=1,usecols=[3,4])
# residuals = np.loadtxt("QL2P08_xray_residuals.txt",dtype='float',delimiter=',',skiprows=1,usecols=[5,6])

# Draw lines on histogram representing quad edges
# Hard coded
xTopLeft = -1806/2.0
yTopLeft = 1194.6
xTopRight = abs(xTopLeft)
yTopRight = yTopLeft
xBotLeft = xTopLeft + yTopLeft*np.tan(np.pi*(28/2)/180) 
yBotLeft = 0
xBotRight = xTopRight -yTopRight*np.tan(np.pi*(28/2)/180)
yBotRight = 0
# Left edge
xLeft = np.array([xTopLeft, xBotLeft])
yLeft = np.array([yTopLeft, yBotLeft])
gLeft = ROOT.TGraph(2, xLeft, yLeft)
gLeft.SetLineWidth(3)
gLeft.Draw("al")
gLeft.SetTitle(";x [mm];y [mm]")

# Confirmed these are right hist limits with QL2P08 formatted TH2F
gLeft.GetXaxis().SetLimits(-1806/2.0, 1806/2.0)
gLeft.GetHistogram().SetMaximum(1194.6)
gLeft.GetHistogram().SetMinimum(0)

xRight = np.array([xTopRight, xBotRight])
yRight = np.array([yTopRight, yBotRight])
gRight = ROOT.TGraph(2, xRight, yRight)
gRight.SetLineWidth(3)
gRight.Draw("same")

# Get the x-ray points
allData = pd.read_csv("QL2P11_xray_residuals.csv")
# Select only the layer 2, fixed layers 1 and 4 data
dataSubset = allData.loc[((allData['layer']==2) & (allData['fixed_layer_a']==1) & (allData['fixed_layer_b']==3)),['x', 'y', 'residual', 'residual error']]
print(dataSubset)
numPoints = len(dataSubset.index)

# testArrow proves that x1,y1,x2,y2 are in same coordinates as graph axes.
# testArrow = ROOT.TArrow(0,0,0,1194.6)
# testArrow.Draw()
arrows = []
annotations = []

for i in range(numPoints):
    # The x500 is a scale factor for drawing the arrow
    arrow = ROOT.TArrow(dataSubset['x'].iloc[i], dataSubset['y'].iloc[i], dataSubset['x'].iloc[i],
                        dataSubset['y'].iloc[i] + dataSubset['residual'].iloc[i]*500, 
                        0.01, ">")
    arrows.append(arrow)
    arrow.Draw()
    noteText = '  {:.2f}'.format(dataSubset['residual'].iloc[i])
    note = ROOT.TLatex(dataSubset['x'].iloc[i], dataSubset['y'].iloc[i], noteText)
    note.SetTextSize(20)
    annotations.append(note)
    note.Draw()


