CXX=g++
ROOTFLAGS=`root-config --cflags --glibs` -L$(ROOTSYS)/lib
ROOTFLAGSLINK=`root-config --cflags` 
CFLAGS=-std=gnu++1y
DEBUG=-Wall -g
EXEC=StripPosAnalysis

CPP := $(wildcard *.cpp)

LINKDEFH := tgc_analysis/CosmicsAnalysis/ContainerLinkDef.h
LINKDEFCPP := ContainerLinkDef.cpp
LINKDEFO := ContainerLinkDef.o
CPP += $(LINKDEFCPP)

DEPS := $(wildcard *.h)

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

# %.o : %.cpp $(DEPS) 
%.o : %.cpp $(DEPS) 
	echo object
	$(CXX) $(ROOTFLAGSLINK) $(CFLAGS) $(DEBUG) -c $< -o $@

$(LINKDEFO) : $(LINKDEFCPP) $(DEPS)
	echo linkdefobj
	$(CXX) $(ROOTFLAGSLINK) $(CFLAGS) $(DEBUG) -c $< -o $@

$(LINKDEFCPP) : $(DEPS) $(LINKDEFH)
	echo linkdefcpp
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

