# For all analysis output local data csv files input to this script, plot the mean cosmics residuals vs x-ray residuals in scatter plots by tracking combination. This is to be able to examine the correlation over all quadruplets of a certain group (e.g. geometry, which wheel / x-ray gun mount type). Each function styles teh plots in a different way, as described by teh comment above the function. The input is a text file containing a list of the names of the csv files of quadruplets you would like to include in the plots.
import ROOT
import numpy as np
import atlasplots as aplt
import sys
from datetime import datetime
import os
import pandas as pd

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

        self.string = "layer" + str(self.layer) + "_fixedlayers" + str(self.la) + str(self.lb)
        self.prettyString = "Layer: " + str(self.layer) + ", fixed layers: " + str(self.la) + ", " + str(self.lb)

# Defines a list of tracking combinations
# Useful for looping over all combinations
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


# For each quadruplet's data input (in inFileNames, the list of csv files to analyze), create the all-combination correlation plots per quadruplet that are usually output from cosmics_xray_correlation, but those calculated at gun position 8B. 
# Gun position 8B is of particular interest because it was found to often yield very large x-ray residuals in tracking combinations including layer 3.
def colorGunPositions(inFileNames, tag, outDir):
    c = ROOT.TCanvas("c","c",800,600)
    outFileNameBase = outDir + "/" + tag + "_colour_by_gun_position"
    c.Print(outFileNameBase + ".pdf[")

    # For each input file, create one correlation plot and print it to pdf
    for fileName in inFileNames:

        data = pd.read_csv(fileName, index_col=False)

        # x8B = data.loc[ (data.loc[:,'X-ray pt id']=="8B"), 'x-ray residual'].to_numpy()
        # ex8B = data.loc[ (data.loc[:,'X-ray pt id']=="8B"), 'xray residual error'].to_numpy()
        # y8B = data.loc[ (data.loc[:,'X-ray pt id']=="8B"), 'Mean cosmics residual'].to_numpy()
        # ey8B = np.sqrt( np.power(data.loc[ (data.loc[:,'X-ray pt id']=="8B"), 'stat error'].to_numpy(), 2) + np.power(data.loc[ (data.loc[:,'X-ray pt id']=="8B"), 'sys error'].to_numpy(), 2) )
        # g8B = ROOT.TGraphErrors(len(x8B), x8B, y8B, ex8B, ey8B)
        # g8B.SetMarkerColor(9)

        # x = data.loc[ (data.loc[:,'X-ray pt id']!="8B"), 'x-ray residual'].to_numpy()
        # ex = data.loc[ (data.loc[:,'X-ray pt id']!="8B"), 'xray residual error'].to_numpy()
        # y = data.loc[ (data.loc[:,'X-ray pt id']!="8B"), 'Mean cosmics residual'].to_numpy()
        # ey = np.sqrt( np.power(data.loc[ (data.loc[:,'X-ray pt id']!="8B"), 'stat error'].to_numpy(), 2) + np.power(data.loc[ (data.loc[:,'X-ray pt id']!="8B"), 'sys error'].to_numpy(), 2) )
        # gOther = ROOT.TGraphErrors(len(x), x, y, ex, ey)
        x8B = data.loc[ (data.loc[:,'X-ray pt id']=="8B"), 'x-ray residual'].to_numpy()
        ex8B = data.loc[ (data.loc[:,'X-ray pt id']=="8B"), 'xray residual error'].to_numpy()
        y8B = data.loc[ (data.loc[:,'X-ray pt id']=="8B"), 'Mean cosmics residual'].to_numpy()
        ey8B = np.sqrt( np.power(data.loc[ (data.loc[:,'X-ray pt id']=="8B"), 'stat error'].to_numpy(), 2) + np.power(data.loc[ (data.loc[:,'X-ray pt id']=="8B"), 'sys error'].to_numpy(), 2) )
        g8B = ROOT.TGraphErrors(len(x8B), x8B, y8B, ex8B, ey8B)
        g8B.SetMarkerColor(9)

        x = data.loc[ (data.loc[:,'X-ray pt id']!="8B"), 'x-ray residual'].to_numpy()
        ex = data.loc[ (data.loc[:,'X-ray pt id']!="8B"), 'xray residual error'].to_numpy()
        y = data.loc[ (data.loc[:,'X-ray pt id']!="8B"), 'Mean cosmics residual'].to_numpy()
        ey = np.sqrt( np.power(data.loc[ (data.loc[:,'X-ray pt id']!="8B"), 'stat error'].to_numpy(), 2) + np.power(data.loc[ (data.loc[:,'X-ray pt id']!="8B"), 'sys error'].to_numpy(), 2) )
        gOther = ROOT.TGraphErrors(len(x), x, y, ex, ey)


        c.cd()
        mg = ROOT.TMultiGraph()
        mg.Add(gOther, "P")
        mg.Add(g8B, "P")
        mg.GetXaxis().SetLimits(-1.5,1.5)
        mg.SetMaximum(1.5)
        mg.SetMinimum(-1.5)
        mg.GetXaxis().SetTitle("X-ray residual [mm]")
        mg.GetYaxis().SetTitle("Mean cosmics residual [mm]")
        mg.Draw("A")

        # Typically, file names start with the quadruplet name
        # (if you make your tags in cosmics_xray_correlation of the format eg. QL2P01_3100V_date_)
        # THIS IS NOT VERY ROBUST
        fileNamePathRemoved = fileName.split('/')[-1]
        quadStr = fileNamePathRemoved.split('_')[0]
        print(quadStr)
        labelBox = ROOT.TPaveText(0.2, 0.8, 0.4, 0.9, "ndc")
        labelBox.SetBorderSize(0)
        labelBox.SetFillStyle(0)
        labelText = labelBox.AddText(quadStr)
        labelText.SetTextSize(30)
        labelBox.Draw()
        c.Print(outFileNameBase + ".pdf")

        # g = ROOT.TGraphErrors(len(data.index))
        # g.GetXaxis().SetLimits(-2,2)
        # g.SetMaximum(2)
        # g.SetMinimum(-2)
        # g.Draw("AP")
        # c.Print(outFileNameBase + ".pdf")

    c.Print(outFileNameBase + ".pdf]")

# Make correlation plots by combination for data in all input csvs
# Outputs them to pdf.
# allData is a pandas dataframe where the data from all input csvs have been concatenated in the global body of this script
def correlationPlotsByCombination(allData, tag, outDir):
    outFileNameBase = outDir + "/" + tag + "_correlation_plots_by_combination"
    c = ROOT.TCanvas("c", "c", 800, 600)
    c.cd()
    c.Print(outFileNameBase + ".pdf[")
    combos = GetComboList()

    for combo in combos:
        x = allData.loc[ ( (allData.loc[:, 'Layer']==combo.layer) & (allData.loc[:, 'Fixed layer 1']==combo.la) & (allData.loc[:, 'Fixed layer 2']==combo.lb) ), 'x-ray residual' ].to_numpy()
        ex = allData.loc[ ( (allData.loc[:, 'Layer']==combo.layer) & (allData.loc[:, 'Fixed layer 1']==combo.la) & (allData.loc[:, 'Fixed layer 2']==combo.lb) ), 'xray residual error' ].to_numpy()
        y = allData.loc[ ( (allData.loc[:, 'Layer']==combo.layer) & (allData.loc[:, 'Fixed layer 1']==combo.la) & (allData.loc[:, 'Fixed layer 2']==combo.lb) ), 'Mean cosmics residual' ].to_numpy()
        eyStat =  allData.loc[ ( (allData.loc[:, 'Layer']==combo.layer) & (allData.loc[:, 'Fixed layer 1']==combo.la) & (allData.loc[:, 'Fixed layer 2']==combo.lb) ), 'stat error' ].to_numpy()
        eySys =  allData.loc[ ( (allData.loc[:, 'Layer']==combo.layer) & (allData.loc[:, 'Fixed layer 1']==combo.la) & (allData.loc[:, 'Fixed layer 2']==combo.lb) ), 'sys error' ].to_numpy()
        ey = np.sqrt(np.power(eyStat,2) + np.power(eySys,2))

        g = ROOT.TGraphErrors(len(x), x, y, ex, ey)
        g.GetXaxis().SetLimits(-2,2)
        g.SetMaximum(2)
        g.SetMinimum(-2)
        g.Draw("AP")
        labelBox = ROOT.TPaveText(0.2,0.8,0.6,0.9,"NDC")
        labelBox.SetFillStyle(0)
        labelBox.SetBorderSize(0)
        labelText = labelBox.AddText(combo.prettyString)
        labelText.SetTextSize(30)
        labelBox.Draw()
        c.Print(outFileNameBase + ".pdf")
    
    c.Print(outFileNameBase + ".pdf]")

# Make correlation plots by combination for data in all input csvs and colors entries taken at
# one of the gun positions on the input list, colorTheseGunPositions (stored as strings, e.g. "8B")
# allData is a pandas dataframe where the data from all input csvs have been concatenated
def correlationPlotsByCombinationColouredGunPositions(allData, colorTheseGunPositions, tag, outDir):
    outFileNameBase = outDir + "/" + tag + "_correlation_plots_by_combination"
    c = ROOT.TCanvas("c", "c", 800, 600)
    c.cd()
    c.SetFillStyle(4000)
    c.Print(outFileNameBase + ".pdf[")
    # ROOT color codes that you can cycle through that produce colours with enough contrast
    # Assigned QL2 gun positions to a colour
    colorByGunPosition = {"9A":2, "11C":3, "5C":4, "8C":5, "8D2":6, "11B":7, "8B":8, "8A":9,
            "7C":13, "11A":28, "6A":30, "4C":38, "5A":40, "7D1":41, "4D1":42, "8D1":47}
    # Same  for marker styles
    markByGunPosition = {"9A":20, "11C":21, "5C":23, "8C":24, "8D2":25, "11B":26, "8B":27, "8A":28,
            "7C":29, "11A":30, "6A":31, "4C":32, "5A":33, "7D1":34, "4D1":39, "8D1":40}
    # acceptableColors = [2,3,4,5,6,7,8,9,13,28,30,38,40,41,42,46,47]
    # colorPerGunPos = acceptableColors[:len(colorTheseGunPositions)]
    combos = GetComboList()

    for combo in combos:
        # This will be the colour of the datapoints of the first input gun position
        # colorNum increments by one for each gun position to highlight colours
        # colorNum = 0
        graphs = []
        labelTexts = [] 

        labelBox = ROOT.TPaveText(0.2,0.2,0.6,0.9,"NDC")
        labelBox.SetFillStyle(0)
        labelBox.SetBorderSize(0)
        labelText = labelBox.AddText(combo.prettyString)
        labelText.SetTextSize(30)
        labelText.SetTextAlign(11)
        labelTexts.append(labelText)

        # Make a graph for all residual pairs for given combination
        # x = allData.loc[ ( (allData.loc[:, 'Layer']==combo.layer) & (allData.loc[:, 'Fixed layer 1']==combo.la) & (allData.loc[:, 'Fixed layer 2']==combo.lb) ), 'x-ray residual' ].to_numpy()
        # ex = allData.loc[ ( (allData.loc[:, 'Layer']==combo.layer) & (allData.loc[:, 'Fixed layer 1']==combo.la) & (allData.loc[:, 'Fixed layer 2']==combo.lb) ), 'xray residual error' ].to_numpy()
        # y = allData.loc[ ( (allData.loc[:, 'Layer']==combo.layer) & (allData.loc[:, 'Fixed layer 1']==combo.la) & (allData.loc[:, 'Fixed layer 2']==combo.lb) ), 'Mean cosmics residual' ].to_numpy()
        # eyStat =  allData.loc[ ( (allData.loc[:, 'Layer']==combo.layer) & (allData.loc[:, 'Fixed layer 1']==combo.la) & (allData.loc[:, 'Fixed layer 2']==combo.lb) ), 'stat error' ].to_numpy()
        # eySys =  allData.loc[ ( (allData.loc[:, 'Layer']==combo.layer) & (allData.loc[:, 'Fixed layer 1']==combo.la) & (allData.loc[:, 'Fixed layer 2']==combo.lb) ), 'sys error' ].to_numpy()
        # ey = np.sqrt(np.power(eyStat,2) + np.power(eySys,2))
        # g = ROOT.TGraphErrors(len(x), x, y, ex, ey)
        # graphs.append(g)

        line = ROOT.TF1("line", "x", -2, 2)
        line.SetLineColor(16)

        # Make separate graphs for residuals taken at gun position for given combo
        for colorNum, gunPos in enumerate(colorTheseGunPositions):
            xg = allData.loc[ ( (allData.loc[:, 'Layer']==combo.layer) & (allData.loc[:, 'Fixed layer 1']==combo.la) & (allData.loc[:, 'Fixed layer 2']==combo.lb) & (allData.loc[:, 'X-ray pt id']==gunPos) ), 'x-ray residual' ].to_numpy()
            exg = allData.loc[ ( (allData.loc[:, 'Layer']==combo.layer) & (allData.loc[:, 'Fixed layer 1']==combo.la) & (allData.loc[:, 'Fixed layer 2']==combo.lb) & (allData.loc[:, 'X-ray pt id']==gunPos) ), 'xray residual error' ].to_numpy()
            yg = allData.loc[ ( (allData.loc[:, 'Layer']==combo.layer) & (allData.loc[:, 'Fixed layer 1']==combo.la) & (allData.loc[:, 'Fixed layer 2']==combo.lb) & (allData.loc[:, 'X-ray pt id']==gunPos) ), 'Mean cosmics residual' ].to_numpy()
            eygStat =  allData.loc[ ( (allData.loc[:, 'Layer']==combo.layer) & (allData.loc[:, 'Fixed layer 1']==combo.la) & (allData.loc[:, 'Fixed layer 2']==combo.lb) & (allData.loc[:, 'X-ray pt id']==gunPos) ), 'stat error' ].to_numpy()
            eygSys =  allData.loc[ ( (allData.loc[:, 'Layer']==combo.layer) & (allData.loc[:, 'Fixed layer 1']==combo.la) & (allData.loc[:, 'Fixed layer 2']==combo.lb) & (allData.loc[:, 'X-ray pt id']==gunPos) ), 'sys error' ].to_numpy()
            eyg = np.sqrt(np.power(eygStat,2) + np.power(eygSys,2))
            if (len(xg)==0):
                print("Warning: no entries found for gun position: ", gunPos) 
                continue
            g = ROOT.TGraphErrors(len(xg), xg, yg, exg, eyg)
            g.SetMarkerStyle(markByGunPosition[gunPos])
            g.SetMarkerColor(colorByGunPosition[gunPos])
            g.SetLineColor(colorByGunPosition[gunPos])
            graphs.append(g)
            gunLabelText = labelBox.AddText(gunPos)
            gunLabelText.SetTextColor(colorByGunPosition[gunPos])
            gunLabelText.SetTextSize(30)
            gunLabelText.SetTextAlign(11)
            labelTexts.append(gunLabelText)
            # Change colours for next gun position
            # colorNum += 1
            # if colorNum==10:
            #    colorNum += 1



        mg = ROOT.TMultiGraph()
        for graph in graphs:
            mg.Add(graph, "P")
        mg.GetXaxis().SetLimits(-2,2)
        mg.SetMaximum(2)
        mg.SetMinimum(-2)
        mg.GetXaxis().SetTitle("X-ray residual [mm]")
        mg.GetYaxis().SetTitle("Mean cosmics residual [mm]")
        mg.Draw("A")
        line.Draw("same")
        labelBox.Draw()
        c.Print(outFileNameBase + ".pdf")
    
    c.Print(outFileNameBase + ".pdf]")

### MAIN ###
aplt.set_atlas_style()
ROOT.gStyle.SetOptStat(1)
ROOT.gROOT.ForceStyle()
# List of names of analysis output csv files (from CompareData class) to include in analysis
inFileList = []
# Get cmd line args
# Just do skeleton for today, Dec. 1 so you can make some distributions
pos = 1
theTag = ""
theOutDir = "out"
gotListOfInputFiles = False
while pos < len(sys.argv):
    # Deal with input file list in .txt file
    if sys.argv[pos] == "-l":
        fileListFile = open(sys.argv[pos+1])
        for fileName in fileListFile:
            inFileList.append(fileName[:-1])
        gotListOfInputFiles = True
        pos += 2
    elif sys.argv[pos] == "--tag":
        theTag = sys.argv[pos+1]
        pos += 2
    elif sys.argv[pos] == "-o":
        if os.path.isdir(sys.argv[pos+1]):
            theOutDir = sys.argv[pos+1]
        else:
            print("Warning: Output directory specified does not exist.\n")
        pos += 2
    else:
        print("Usage: python regroupingCorrelationPlots.py -l list_of_input_csv_files.txt [-o outputDirectoryName] [--tag outputFileNamePrefix]\n")
        exit()

# If no tag has been assigned
if theTag == "":
    theTag = "regrouping_correlation_plots_" + datetime.today().strftime('%Y-%m-%d')
print("Output will be put in", theOutDir, "\n")

if gotListOfInputFiles == False:
    print("Usage: python regroupingCorrelationPlots.py -l list_of_paths_to_input_files.txt [-o output directory] [--tag append_this_prefix_to_output_filenames]")
    exit(1)
if len(inFileList)==0:
    print("List of input files is empty.\n")
    exit(1)

# colorGunPositions(inFileList, theTag, theOutDir)

# allTheData = pd.read_csv(inFileList[0], index_col=False)
# For every input file, put all rows in csv in dataframe and add column listing input file name
df = pd.read_csv(inFileList[0], index_col=False)
# df['inFileName']= inFileList[0]
df['quad'] = inFileList[0].split('/')[-1].split('_')[0]
df['Mean cosmics residual error'] = (df['stat error'].pow(2) + df['sys error'].pow(2)).pow(1./2)
allTheData = df
for i in range(1, len(inFileList)):
    df = pd.read_csv(inFileList[i], index_col=False)
    # df['inFileName']= inFileList[i]
    df['quad'] = inFileList[i].split('/')[-1].split('_')[0]
    df['Mean cosmics residual error'] = (df['stat error'].pow(2) + df['sys error'].pow(2)).pow(1./2)
    allTheData = allTheData.append(df, ignore_index=True)
allTheData.to_csv(theOutDir + "/" + theTag + "_correlation_data.csv", columns=['quad', 'X-ray pt id', 'Layer', 'Fixed layer 1', 'Fixed layer 2', 'x-ray residual', 'xray residual error', 'Mean cosmics residual', 'Mean cosmics residual error'], header=['Quad', 'Gun position', 'Layer', 'Fixed layer 1', 'Fixed layer 2', 'X-ray residual', 'X-ray residual error', 'Mean cosmics residual', 'Mean cosmics residual error'], index=False)

gunPositions = allTheData['X-ray pt id'].unique()
# gunPositions=["9A","8D1","8D2","8A"]
# gunPositions=["11B","7C"]
print(gunPositions)
correlationPlotsByCombinationColouredGunPositions(allTheData, gunPositions, theTag, theOutDir)
