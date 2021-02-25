//***********************************************************
// ReClustering takes a tracks TTree from CosmicsAnalysis and
// redoes the cluster fit.
// It outputs the cluster parameters in a reformatted TTree
// with more information about the fit than the original, 
// namely, the fit parameter uncertainties.
//
// Author: Lia Formenti
// Date: 2021-02-18
// lia.formenti@mail.mcgill.ca
// ************************************************************

#ifndef ReClustering_h
#define ReClustering_h

// C++ include
#include <iostream>
#include <vector>
#include <map>

// ROOT includes
#include <TROOT.h>
#include <TSystem.h>
#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TH1F.h>
#include <TMath.h>

// tgc_analysis includes
#include "PlotManager.h"

// My includes
#include "Helper.h"

/*class ReClusteringPlots {
  public:
      ReClusteringPlots();
      ReClusteringPlots(TTree* reclusteredTree);
      ~ReClusteringPlots();

};*/

/*void makeReClusteringPlots(TTree* rTree);*/

#endif
