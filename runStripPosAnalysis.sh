#!/bin/bash

# User specific aliases and functions
export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase
source ${ATLAS_LOCAL_ROOT_BASE}/user/atlasLocalSetup.sh
localSetupROOT 6.14.04-x86_64-slc6-gcc62-opt
export ALRB_rootVersion=6.14.04-x86_64-slc6-gcc62-opt

"/afs/cern.ch/user/l/liformen/private/strip_position_analysis/StripPosAnalysis" /eos/atlas/atlascerngroupdisk/det-nsw-stgc/cosmics_test_bench_data/QS3P/QS3P07/ANALYSIS_2900V/CosmicsAnalysis.root
