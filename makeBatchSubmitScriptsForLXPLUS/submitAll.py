# Submits all .sub files in liformen@lxplus.cern.ch:~/private/submitScripts to HTCondor high performance computing (batch) system on LXPLUS.
# To be run on LXPLUS
# python3 submitAll.py

import os
import glob

# Set isTest to False to actually submit
# Set isTest to True to print commands that will be executed in terminal to stdout
isTest = False 

### Change this to where you keep your .sub files to run
### Note that you can change the wildcard at the end to select only specific .sub files
subFilePath = "/afs/cern.ch/user/l/liformen/private/submitScripts/*.sub"
# For testing on your local machine (not on LXPLUS)
# subFilePath = "./*.sub"
subFileNamesList = glob.glob(subFilePath)
commandBase = "condor_submit"

for subFile in subFileNamesList:
    if isTest:
        print(commandBase + ' ' + subFile)
    else:
        # Actually run the commmand
        os.system(commandBase + ' ' + subFile)
