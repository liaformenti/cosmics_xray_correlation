# Takes in a the local data csv file output from cosmics_xray_correlation and remakes the all-tracking combination correlation plot, styled as figure 7.2 and figure 7.3 from Lia's thesis. Colours each entry by whether it corresponds to an interpolation or extrapolation tracking combination.

import ROOT
import numpy as np
import pandas as pd
import atlasplots as aplt
import sys
import uncertainties as u

def makeCorrelationByColorPlot(inFileName):

    # Paul Tol high contrast colours - good for colour blind and greyscale printing
    # Doesn't work
    # ROOT.gStyle.SetColorModelPS(0)
    # colorIndexBlue = ROOT.TColor.GetFreeColorIndex()
    # tHighConstBlue = ROOT.TColor(colorIndexBlue, 0, 68./255, 136./255)
    # colorIndexYel = ROOT.TColor.GetFreeColorIndex()
    # tHighConstYel = ROOT.TColor(colorIndexYel, 221./255, 170./255, 51./255)
    
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
    # interpGraph.SetMarkerColor(38) # soft blue
    interpGraph.SetMarkerSize(0)
    # interpGraph.SetLineColor(616+3)
    # interpGraph.SetLineStyle(2)
    interpGraph.SetLineWidth(0)
    interpGraph.SetFillColorAlpha(416-3, 0.35)
    # interpGraph.SetFillColorAlpha(colorIndexYel, 0.35)
    interpGraph.SetTitle("Interpolation")

    xe = extrapData.loc[:,'x-ray residual'].to_numpy()
    exe = extrapData.loc[:,'xray residual error'].to_numpy()
    ye = extrapData.loc[:,'Mean'].to_numpy()
    eyeStat = extrapData.loc[:,'Mean error'].to_numpy()
    eyeSys = extrapData.loc[:,'sys error'].to_numpy()
    eye = np.sqrt(np.power(eyeStat,2)+np.power(eyeSys,2))
    extrapGraph = ROOT.TGraphErrors(xe.size, xe, ye, exe, eye)
    # extrapGraph.SetMarkerColor(416+3) # forest green
    extrapGraph.SetMarkerSize(0)
    extrapGraph.SetLineWidth(0)
    extrapGraph.SetFillColorAlpha(880+4, 0.35) # violet
    # extrapGraph.SetFillColorAlpha(colorIndexBlue, 0.35) # blue
    extrapGraph.SetTitle("Extrapolation")
    
    c = ROOT.TCanvas("c","c",800,600)
    c.cd()

    mg = ROOT.TMultiGraph()
    mg.Add(extrapGraph, "2")
    mg.Add(interpGraph, "2")
    mg.GetXaxis().SetLimits(-1.5,1.5)
    mg.SetMaximum(1.5)
    mg.SetMinimum(-1.5)
    mg.SetTitle("#splitline{Comparing residuals}{All tracking combinations}")
    # mg.GetXaxis().SetTitle("Residual from x-ray data [mm]")
    # mg.GetYaxis().SetTitle("Mean residual from cosmics [mm]")


    # Do fit
    linFunc = ROOT.gROOT.GetFunction("pol1")
    linFunc.SetParameters(0,1)
    linFunc.SetParNames("Offset", "Slope")
    linFunc.SetLineWidth(1)
    linFunc.SetLineColor(920+1)
    mg.Fit(linFunc, "0")

    linFunc.SetMaximum(1.5)
    linFunc.SetMinimum(-1.5)
    linFunc.SetRange(-1.5,1.5)
    linFunc.SetTitle("Fit")
    linFunc.GetXaxis().SetTitle("X-ray residual [mm]")
    linFunc.GetYaxis().SetTitle("Mean cosmics residual [mm]")
    linFunc.SetMarkerSize(0)
    
    linFunc.Draw()

    # Get fit params
    offset = u.ufloat(linFunc.GetParameter(0), linFunc.GetParError(0))
    slope = u.ufloat(linFunc.GetParameter(1), linFunc.GetParError(1))
    chi2 = u.ufloat(linFunc.GetChisquare(), 0.1)
    chi2Str = "{}".format(chi2).split('+')[0]
    ndf = linFunc.GetNDF()

    # format fit params
    # fitBox = ROOT.TPaveText(0.5,0.2,0.8,0.4,"NDC")
    # fitBox = ROOT.TPaveText(0.6,0.69,0.9,0.9,"NDC")
    fitBox = ROOT.TPaveText(0.6, 0.2, 0.9, 0.55, "NDC")
    interpLine = fitBox.AddText("Interpolation")
    interpLine.SetTextColor(416-3)
    # interpLine.SetTextColor(colorIndexYel)
    interpLine.SetTextSize(20)
    interpLine.SetTextAlign(32)
    extrapLine = fitBox.AddText("Extrapolation")
    extrapLine.SetTextColor(880+4)
    # extrapLine.SetTextColor(colorIndexBlue)
    extrapLine.SetTextSize(20)
    extrapLine.SetTextAlign(32)
    chi2Line = fitBox.AddText("\chi^{2}/ndf " + " {}/{}".format(chi2Str,ndf))
    chi2Line.SetTextColor(920+1)
    chi2Line.SetTextSize(20)
    chi2Line.SetTextAlign(32)
    offsetLine = fitBox.AddText("Offset {} mm".format(offset))
    offsetLine.SetTextColor(920+1)
    offsetLine.SetTextSize(20)
    offsetLine.SetTextAlign(32)
    slopeLine = fitBox.AddText("Slope {}".format(slope))
    slopeLine.SetTextColor(920+1)
    slopeLine.SetTextSize(20)
    slopeLine.SetTextAlign(32)
    fitBox.SetBorderSize(0)
    fitBox.SetFillColorAlpha(0,1)


    # Legend
    # c.BuildLegend(0.2,0.69,0.5,0.9);
    # c.BuildLegend(0.6,0.2,0.9,0.4)
    # legend = ROOT.TLegend(0.35,0.69,0.65,0.9)
    # legend = ROOT.TLegend(0.6, 0.35, 0.9, 0.55)
    # legend.AddEntry(interpGraph, "Interpolation", "f")
    # legend.AddEntry(extrapGraph, "Extrapolation", "f")
    # legend.SetTextSize(20)
    # legend.SetFillColorAlpha(0,1)

    # legend.Draw()
    fitBox.Draw()

    mg.Draw()

    # Print
    c.Print("figure_" + inFileName[:-4] + "_correlation_plot.pdf")
    outFile = ROOT.TFile("figure_" + inFileName[:-4] + "_correlation_plot.root", "RECREATE")
    c.Write(inFileName[:-4] + "_correlation_plot")
    return

if (len(sys.argv) < 2):
    print("Usage: python colorByInterpExtrap.py inFileName.csv")
    exit()

aplt.set_atlas_style()
style = ROOT.gROOT.GetStyle("ATLAS")
# style.SetOptFit(111)
makeCorrelationByColorPlot(sys.argv[1])
