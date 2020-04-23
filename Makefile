CXX=g++
ROOTFLAGS=`root-config --cflags --glibs` -L$(ROOTSYS)/lib
StripPosAnalysis: StripPosAnalysis.cpp
	$(CXX) $(ROOTFLAGS) StripPosAnalysis.cpp -o StripPosAnalysis.o
# From CosmicsAnalysis Makefile
# ROOTFLAGS=`root-config --cflags --glibs` -lRooFitCore  -lTMVA -lRooFit -lHtml -lMinuit -L$(ROOTSYS)/lib -L$(ROOTSYS)/tmva
# ROOTFLAGSLINK=`root-config --cflags`
# CFLAGS=-std=gnu++1y -lsqlite3 `xml2-config --cflags` -lxml2

