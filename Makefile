CXX=g++
ROOTFLAGS=`root-config --cflags --glibs` -lRooFitCore  -lTMVA -lRooFit -lHtml -lMinuit -L$(ROOTSYS)/lib -L$(ROOTSYS)/tmva
ROOTFLAGSLINK=`root-config --cflags`
CFLAGS=-std=gnu++1y -lsqlite3 `xml2-config --cflags` -lxml2
DEBUG=-Wall -g
EXEC=StripPosAnalysis

CPP  := $(wildcard *.cpp)

LINKDEFH := $(wildcard *LinkDef.h)
LINKDEFCPP := $(LINKDEFH:.h=.cpp)
CPP += $(LINKDEFCPP)

DEPS := $(wildcard *.h)
DEPS := $(filter-out $(LINKDEFH), $(DEPS))

OBJ := $(CPP:.cpp=.o)
OBJ := $(filter-out StripPosAnalysis.o,$(OBJ))

all: $(EXEC)

optimized: | process-optimized all

process-optimized:
        $(eval CFLAGS += -O1 -g)

StripPosAnalysis: $(OBJ) StripPosAnalysis.o
	$(CXX) $(ROOTFLAGS) $(CFLAGS) $(DEBUG) $^ -o $@

%.o : %.cpp $(DEPS)
	$(CXX) $(ROOTFLAGSLINK) $(CFLAGS) $(DEBUG) -c $< -o $@

%LinkDef.cpp: $(DEPS) %LinkDef.h
	rootcint -f $@ -c $(CFLAGS) $(ROOTFLAGSLINK) -p $^

clean:
	rm -rf *.o *LinkDef.cpp $(EXEC) *-e *_rdict.pcm


.PHONY: clean
