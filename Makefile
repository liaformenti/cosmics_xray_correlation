CXX=g++
ROOTFLAGS=`root-config --cflags --glibs` -L$(ROOTSYS)/lib
ROOTFLAGSLINK=`root-config --cflags` 
CFLAGS=-std=gnu++1y -lsqlite3  `xml2-config --cflags` -lxml2
DEBUG=-Wall -g
EXEC=StripPosAnalysis

CPP := $(wildcard *.cpp)

LINKDEFH := tgc_analysis/CosmicsAnalysis/AnalysisInfoLinkDef.h tgc_analysis/CosmicsAnalysis/RunInfoLinkDef.h tgc_analysis/CosmicsAnalysis/SectorLinkDef.h tgc_analysis/CosmicsAnalysis/ContainerLinkDef.h 

LINKDEFCPP := ContainerLinkDef.cpp AnalysisInfoLinkDef.cpp RunInfoLinkDef.cpp SectorLinkDef.cpp
# LINKDEFO := $(LINKDEFCPP:.cpp=.o)
CPP += $(LINKDEFCPP)

DEPS := $(wildcard *.h)
DEPS += tgc_analysis/CosmicsAnalysis/AnalysisInfo.h

OBJ := $(CPP:.cpp=.o)
OBJ := $(filter-out StripPosAnalysis.o,$(OBJ))
# OBJ := $(filter-out $(LINKDEFO),$(OBJ))

# check:
#	echo $(OBJ)

all: $(EXEC)
	echo all

StripPosAnalysis : $(OBJ) StripPosAnalysis.o
	echo stripposanalysis
	$(CXX) $(ROOTFLAGS) $(CFLAGS) $(DEBUG) $^ -o $@

# %LinkDef.o : %LinkDef.cpp $(DEPS)
#	$(CXX) $(ROOTFLAGSLINK) $(CFLAGS) $(DEBUG) -c $< -o $@

%.o : %.cpp $(DEPS) 
	echo $(LINKDEFCPP)
	$(CXX) $(ROOTFLAGSLINK) $(CFLAGS) $(DEBUG) -c $< -o $@

# Put this above %.o so hopefully it being first will prevent %.o rule from going first
# $(LINKDEFO) : $(LINKDEFCPP) $(DEPS)
#	echo linkdefobj
#	$(CXX) $(ROOTFLAGSLINK) $(CFLAGS) $(DEBUG) -c $< -o $@

# $(LINKDEFCPP) : $(DEPS) $(LINKDEFH)
#	echo linkdefcpp
#	rootcint -f $@ -c $(CFLAGS) $(ROOTFLAGSLINK) -p $^

%LinkDef.cpp: $(DEPS) tgc_analysis/CosmicsAnalysis/%LinkDef.h
	rootcint -f $@ -c $(CFLAGS) $(ROOTFLAGSLINK) -p $^

clean:
	rm -rf *.o $(LINKDEFCPP) $(EXEC) *_rdict.pcm

.PHONY: clean

# tgc_analysis/ContainerLinkDef.cpp : tgc_analysis/ContainerLinkDef.h
#	rootcint -f $@ -c $(ROOTFLAGSLINK) -p $^

# From CosmicsAnalysis Makefile
# ROOTFLAGS=`root-config --cflags --glibs` -lRooFitCore  -lTMVA -lRooFit -lHtml -lMinuit -L$(ROOTSYS)/lib -L$(ROOTSYS)/tmva
# ROOTFLAGSLINK=`root-config --cflags`
# CFLAGS=-std=gnu++1y -lsqlite3 `xml2-config --cflags` -lxml2

