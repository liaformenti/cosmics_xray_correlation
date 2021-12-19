import os
import glob
from datetime import datetime

# Create scripts to submit ReClustering jobs for multiple quads at once to be run with LXPLUS'
# HTCondor system.
# Run this on LXPLUS
# Coding it locally because it's easier.
# 2021-11-30
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
quadsToRun = [1]
# Both directories must exist
# cosmicsOutDir = "/eos/user/l/liformen/" + quadType + "/"
correlationOutDir = "/eos/user/l/liformen/" + quadType + "/CosmicsXRayCorrelationOutput/"
submitScriptsOutDir = "/afs/cern.ch/user/l/liformen/private/submitScripts/"
# For testing this script with isTest=True (actually writing files) on local machine,
# submitScriptsOutDir = "./"
logOutDir = "/afs/cern.ch/user/l/liformen/private/logs/"
outOutDir = "/afs/cern.ch/user/l/liformen/private/outs/"
errOutDir = "/afs/cern.ch/user/l/liformen/private/errs/"
programInDir = "/afs/cern.ch/user/l/liformen/private/strip_position_analysis/"
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
    tag = quadType + quadNumStr + '_' + str(voltage) + '_' + datetime.today().strftime('%Y-%m-%d') + '_'
    fileNamePrefix = tag + "run_CosmicsXRayCorrelation"
    # Prepare cosmics input file
    reclusteringInFileName = "/eos/user/l/liformen/" + quadType + "/ReClusteringOutput/"
    reclusteringInFileName += quadType + quadNumStr + "_" + voltage
    reclusteringInFileName += "_2021-11-16_" + "reclustering.root"

    # Print notice
    print("For " + quadType + quadNumStr + ' ' + str(voltage) + ":")
    print("----------------------------------------------")
    shFileName = submitScriptsOutDir + fileNamePrefix  + ".sh"
    print()
    if isTest: print(shFileName + ":\n")


    # Prepare execute file text (.sh file)
    shFileText = "#!/bin/bash\n"
    shFileText += "\n"
    shFileText += "# Made with makePremadeCorrelationSubmitScript.sh\n"
    shFileText += "\n"
    shFileText += "export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase\n"
    shFileText += "source ${ATLAS_LOCAL_ROOT_BASE}/user/atlasLocalSetup.sh\n"
    shFileText += "localSetupROOT 6.14.04-x86_64-slc6-gcc62-opt\n"
    shFileText += "export ALRB_rootVersion=6.14.04-x86_64-slc6-gcc62-opt\n"
    shFileText += "\n"
    shFileText += "# Run CosmicsXRayCorrelation -- N.B. executable must be pre-made!\n"
    shFileText += "./CosmicsXRayCorrelation " + reclusteringInFileName + " " + quadType
    shFileText += quadNumStr + " /eos/user/l/liformen/results_2021-11-26.db "
    shFileText += correlationOutDir + " " + tag

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
    subFileText = "# Prepared by makeCorrelationSubmitScript.py\n"
    subFileText += "executable = " + shFileName + '\n'
    subFileText += "output = " + outOutDir + fileNamePrefix + ".out\n"
    subFileText += "error = " + errOutDir + fileNamePrefix + ".err\n"
    subFileText += "log = " + logOutDir + fileNamePrefix + ".log\n"
    subFileText += "requirements = (OpSysAndVer =?= \"CentOS7\")\n"
    subFileText += "+JobFlavour = \"longlunch\"\n"
    subFileText += "transfer_input_files = " + geometryFilePath + ','
    subFileText += programInDir + "CosmicsXRayCorrelation,"
    subFileText += programInDir + "ContainerLinkDef_rdict.pcm,"
    subFileText += programInDir + "SectorLinkDef_rdict.pcm,"
    subFileText += programInDir + "AnalysisInfoLinkDef_rdict.pcm,"
    subFileText += programInDir + "RunInfoLinkDef_rdict.pcm\n"
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
