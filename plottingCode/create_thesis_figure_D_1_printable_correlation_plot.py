# Takes in a the local data csv file output from cosmics_xray_correlation and remakes the all-tracking combination correlation plot, styled as figure D.1 and figure D.2 from Lia's thesis. Colours each entry by whether it corresponds to an interpolation or extrapolation tracking combination.
import ROOT
import numpy as np
import pandas as pd
import atlasplots as aplt
import sys

def makeCorrelationByColorPlot(inFileName):
    data = pd.read_csv(inFileName, index_col=False)
    print(data)
    interpData = data.loc[ (data.loc[:,'Fixed layer 1']<data.loc[:,'Layer']) & (data.loc[:,'Layer']<data.loc[:,'Fixed layer 2']), :]
    extrapData = data.loc[ (data.loc[:,'Fixed layer 1']>data.loc[:,'Layer']) | (data.loc[:,'Layer']>data.loc[:,'Fixed layer 2']), :]
    print(data.shape[0])
    print(interpData.shape[0])
    print(extrapData.shape[0])
    
    xi = interpData.loc[:,'x-ray residual'].to_numpy()
    exi = interpData.loc[:,'xray residual error'].to_numpy()
    yi = interpData.loc[:,'Mean'].to_numpy()
    eyiStat = interpData.loc[:,'Mean error'].to_numpy()
    eyiSys = interpData.loc[:,'sys error'].to_numpy()
    eyi = np.sqrt(np.power(eyiStat,2) + np.power(eyiSys,2))
    interpGraph = ROOT.TGraphErrors(xi.size, xi, yi, exi, eyi)
    interpGraph.SetMarkerColor(416+3) # forest green
    interpGraph.SetMarkerSize(2)
    interpGraph.SetMarkerStyle(22) # triangle
    interpGraph.SetTitle("Interpolation")

    xe = extrapData.loc[:,'x-ray residual'].to_numpy()
    exe = extrapData.loc[:,'xray residual error'].to_numpy()
    ye = extrapData.loc[:,'Mean'].to_numpy()
    eyeStat = extrapData.loc[:,'Mean error'].to_numpy()
    eyeSys = extrapData.loc[:,'sys error'].to_numpy()
    eye = np.sqrt(np.power(eyeStat,2)+np.power(eyeSys,2))
    extrapGraph = ROOT.TGraphErrors(xe.size, xe, ye, exe, eye)
    extrapGraph.SetMarkerColor(880+4) # purple
    extrapGraph.SetMarkerSize(2)
    extrapGraph.SetTitle("Extrapolation")
    
    c = ROOT.TCanvas("c","c",800,600)
    c.cd()
    mg = ROOT.TMultiGraph()
    mg.Add(extrapGraph, "p")
    mg.Add(interpGraph, "p")
    mg.GetXaxis().SetLimits(-1.5,1.5)
    mg.SetMaximum(1.5)
    mg.SetMinimum(-1.5)
    mg.SetTitle("#splitline{Comparing residuals}{All tracking combinations}")
    mg.GetXaxis().SetTitle("Residual from x-ray data [mm]")
    mg.GetYaxis().SetTitle("Mean residual from cosmics [mm]")
    # Do fit
    linFunc = ROOT.gROOT.GetFunction("pol1")
    linFunc.SetParameters(0,1)
    linFunc.SetParNames("Offset", "Slope")
    mg.Fit(linFunc)
    mg.Draw("a")
    linFunc.SetLineColor(920+2)
    # c.BuildLegend(0.3,0.21,0.3,0.21,"","tr");
    legend = ROOT.TLegend(0.6,0.21,0.9,0.41,"", "NDC")
    legLine1 = legend.AddEntry(interpGraph, "Interpolation", "lep")
    legLine1.SetTextAlign(32)
    legLine2 = legend.AddEntry(extrapGraph, "Extrapolation", "lep")
    legLine2.SetTextAlign(32)
    legend.Draw()
    c.Print("figure_" + inFileName[:-4] + "_correlation_plot_printable.pdf")
    outFile = ROOT.TFile("figure_" + inFileName[:-4] + "_correlation_plot.root", "RECREATE")
    c.Write(inFileName[:-4] + "_correlation_plot")
    return

if (len(sys.argv) < 2):
    print("Usage: python colorByInterpExtrap.py inFileName.csv")
    exit()

aplt.set_atlas_style()
style = ROOT.gROOT.GetStyle("ATLAS")
style.SetOptFit(111)
makeCorrelationByColorPlot(sys.argv[1])
