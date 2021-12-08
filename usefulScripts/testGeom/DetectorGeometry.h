#ifndef DetectorGeometry_h
#define DetectorGeometry_h

// C++ includes
#include <vector>
#include <map>
#include <stdexcept>
#include <set>
#include <utility>

// ROOT includes
#include <Rtypes.h>

// My includes
#include "Sector.h"

// This abstract class return the geometry of the sTGC channels
// It also tells if an electrode is connected to a VMM channel or not (i.e. instrumented)

// The channel numbering starts at 0.
// All units are mm.
// Some functions of the dectector geometry depends both on the DAQ and the
// detectors. The instrumented channels change if the adaptor boards are
// modified.
enum Logic : Int_t {fPIVOT=0, fCONFIRM=1};

class DetectorGeometry{

 public:
  DetectorGeometry(){};
  virtual ~DetectorGeometry(){};
  
  // Geometry getters
  virtual std::set<UShort_t> GetLayers()=0;
  virtual UShort_t GetNLayers()=0;
  virtual std::set<Sector> GetSectors()=0;
  virtual Double_t GetZPosition(const UShort_t layer)=0; //(of wire planes)
  virtual Double_t GetPitch(ElectrodeType type);
  virtual UShort_t GetNChannels(const Sector &sector)=0;
  virtual std::vector<Double_t> GetSupportsY(const UShort_t layer)=0;
  virtual std::vector<Double_t> GetSupportsWidth(const UShort_t layer)=0;
  virtual std::pair<Double_t, Double_t> GetModuleLimitsX()=0;
  virtual std::pair<Double_t, Double_t> GetModuleLimitsY()=0;

  // Channel/sector exists
  // Removed concept of instrumented channel
  // may come back is really really needed
  virtual Bool_t IsInstrumented(const Channel &ch)=0;
  virtual Channel FirstInstrumentedChannel(const Sector &sector)=0;
  virtual Channel LastInstrumentedChannel(const Sector &sector)=0;
  virtual Bool_t SectorExists(const Sector &sector)=0;
  virtual Bool_t ChannelExists(const Channel &ch)=0;

  // These functions are necessary because the channel numbering
  // may start from 0 or 1. The channels should not skip a number though.
  virtual Channel GetFirstChannel(const Sector &sector)=0;
  virtual Channel GetLastChannel(const Sector &sector)=0;


  // Get position of channel
  virtual Bool_t InFiducialArea(const Double_t position, const Sector &sector)=0;
  virtual Bool_t InFiducialArea(const Double_t posX, const Double_t posY,
				const Sector &sector)=0;
  virtual Double_t PositionChannel(const Channel &ch)=0; // Except pads
  virtual void PositionPad(const Channel &ch, Double_t &xMin, Double_t &xMax,
			   Double_t &yMin, Double_t &yMax)=0; // Pads only, should be improved with NSW modules
  virtual std::pair<Double_t,Double_t> PositionEdgesChannel(const Channel &ch)=0;
  virtual std::pair<Double_t,Double_t> GetLimitsChannels(const Sector &sector)=0;

  
  // Get channel from position
  virtual UShort_t GetID(const Double_t position, const Sector &sector)=0; //exc pad
  virtual UShort_t GetID(const Double_t xPos, const Double_t yPos,
			    const Sector &sector)=0; // Pads only
  
  // Other functions
  // Mostly useful for pads, trivial for others
  virtual Bool_t AreNeighbours(const Channel &pad1, const Channel &pad2)=0; 
  virtual Bool_t OnStructuralSupport(const Double_t posX, const Double_t posY,
				     const UShort_t layer)=0;

  virtual Bool_t HasLogicalPad(const Double_t posX, const Double_t posY)=0;
  virtual std::string GetLogicalPad(const Double_t posX, 
				    const Double_t posY)=0;
  virtual Bool_t IsLogicalPad(const std::string &logicalPad)=0;
  virtual std::set<std::string> GetAllLogicalPads(const double inc)=0;  
  virtual std::set<std::string> GetAllLogicalPads()=0;
  virtual std::pair<Sector,Sector> GetLogicalPadsReferenceSectors()=0;
  virtual Logic GetPadLogic()=0;
  virtual Int_t GetNPadRows(const UShort_t layer)=0;
  virtual Int_t GetNPadColumns(const UShort_t layer)=0;

  
 protected:
  Double_t stripPitch;
  Double_t wirePitch;
  static std::runtime_error invalidParameters;  
};

#endif
