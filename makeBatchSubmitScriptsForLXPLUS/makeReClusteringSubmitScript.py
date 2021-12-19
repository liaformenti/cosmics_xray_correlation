import os
import glob
from datetime import datetime

# Create scripts to submit ReClustering jobs for multiple quads at once to be run with LXPLUS'
# HTCondor system (creats a .sub file and a .sh file for each quad you want to run).
# This script is written to only do quads of the same geometry and data collected at the same voltage together.
# There is a primer on HTCondor on the ATLAS McGill Twiki.
# Submit jobs with the line condor_submit submitFile.sub
# .sub file will run .sh executable file and create output, log and error files in the directories specified on lines 34-36.
# Run this on LXPLUS
# 2021-11-15
# Run on LXPLUS: python3 makeReClusteringSubmitScript.py

###################################
# Modify these parameters as needed
###################################

# If isTest is true, print the .sh file contents and .sub file contents to stdout
# If isTest is false, actually make the files.
isTest = False 
quadType = "QL2C"
quadTypeLower = quadType.lower()
voltage = "3100V"
# Specify the quad numbers you want to run
quadsToRun = [11, 12]
# Output directories
# All directories must exist
# cosmicsOutDir = "/eos/user/l/liformen/" + quadType + "/"
# Where to put output of ReClustering
reclusteringOutDir = "/eos/user/l/liformen/" + quadType + "/ReClusteringOutput"
# Where to write the .sub and .sh files
submitScriptsOutDir = "/afs/cern.ch/user/l/liformen/private/submitScripts/"
# Where to write the log, out and error files
logOutDir = "/afs/cern.ch/user/l/liformen/private/logs/"
outOutDir = "/afs/cern.ch/user/l/liformen/private/outs/"
errOutDir = "/afs/cern.ch/user/l/liformen/private/errs/"
# Path to the ReClustering executable
programInDir = "/afs/cern.ch/user/l/liformen/private/strip_position_analysis/ReClustering/"
# Path to the tgc_analysis geometry files
geometryFilePath = "/afs/cern.ch/user/l/liformen/private/strip_position_analysis/tgc_analysis/geometry"

######################
# Loop to create files
######################
for quadNum in quadsToRun:

    # Make quadnum a string
    if quadNum < 10: # need to add the preceeding zero
        quadNumStr = "0" + str(quadNum)
    else:
        quadNumStr = str(quadNum)

    # Prepare tag
    tag = quadType + quadNumStr + '_' + str(voltage) + '_' + datetime.today().strftime('%Y-%m-%d')
    fileNamePrefix = tag + "_run_ReClustering"
    # Prepare cosmics input file
    cosmicsInFileName = "/eos/user/l/liformen/" + quadType + "/CosmicsAnalysisOutput" + voltage + "/"
    cosmicsInFileName += quadTypeLower + quadNumStr + "_" + voltage + "_CosmicsAnalysis.root"

    # Print notice
    print("For " + quadType + quadNumStr + ' ' + str(voltage) + ":")
    print("----------------------------------------------")
    shFileName = submitScriptsOutDir + fileNamePrefix  + ".sh"
    print()
    if isTest: print(shFileName + ":\n")


    # Prepare execute file text (.sh file)
    shFileText = "#!/bin/bash\n"
    shFileText += "\n"
    shFileText += "# Made with makeCosmicsSubmitScript.sh\n"
    shFileText += "\n"
    shFileText += "export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase\n"
    shFileText += "source ${ATLAS_LOCAL_ROOT_BASE}/user/atlasLocalSetup.sh\n"
    shFileText += "localSetupROOT 6.14.04-x86_64-slc6-gcc62-opt\n"
    shFileText += "export ALRB_rootVersion=6.14.04-x86_64-slc6-gcc62-opt\n"
    shFileText += "\n"
    shFileText += "# Make ReClustering\n"
    shFileText += "cd " + programInDir + "\n"
    shFileText += "make\n"
    shFileText += "\n"
    shFileText += "# Run ReClustering\n"
    shFileText += "./ReClustering -o " + reclusteringOutDir + ' '
    shFileText += "--tag " + tag + ' '
    shFileText += cosmicsInFileName + '\n'

    if isTest: # Print shFileText to stdout
        print(shFileText)
        print()
    else: # Print shFileText to .sh file
        shFile = open(shFileName, 'w')
        shFile.write(shFileText)
        print(shFileName + " written.")
        print()
        shFile.close()

    # Prepare submit file text (.sub file)
    subFileName = submitScriptsOutDir + fileNamePrefix + ".sub"
    if isTest: print(subFileName + ":\n")

    # Prepare subFileText
    subFileText = "# Prepared by makeCosmicsSubmitScript.py\n"
    subFileText += "executable = " + shFileName + '\n'
    subFileText += "output = " + outOutDir + fileNamePrefix + ".out\n"
    subFileText += "error = " + errOutDir + fileNamePrefix + ".err\n"
    subFileText += "log = " + logOutDir + fileNamePrefix + ".log\n"
    subFileText += "requirements = (OpSysAndVer =?= \"CentOS7\")\n"
    subFileText += "+JobFlavour = \"longlunch\"\n"
    subFileText += "transfer_input_files = " + geometryFilePath + '\n';
    subFileText += "queue"

    if isTest: # print subFileText to stdout
        print(subFileText)
        print()
        print()
    else: # print subFileText to .sub file
        subFile = open(subFileName, 'w')
        subFile.write(subFileText)
        print(subFileName + " written.")
        print()
        print()
        subFile.close()
