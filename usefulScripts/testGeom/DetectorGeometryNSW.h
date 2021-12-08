#ifndef DetectorGeometryNSW_h
#define DetectorGeometryNSW_h

// C++ includes
#include <vector>
#include <map>
#include <cmath>
#include <string>
#include <map>
#include <set>
#include <stdexcept>
#include <limits>
#include <dirent.h>

#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

// ROOT includes
#include <Rtypes.h>

// My includes
#include "DetectorGeometry.h"
#include "Tools.h"

class DetectorGeometryNSW : public DetectorGeometry{
  friend class GeoReader;

 public:
  DetectorGeometryNSW(const std::string _detectortype);
  virtual ~DetectorGeometryNSW(){};

  // Inherited from DetectorGeometry.h
  inline virtual std::set<UShort_t> GetLayers(){return layers;};
  inline virtual UShort_t GetNLayers();
  virtual std::set<Sector> GetSectors();
  virtual Double_t GetZPosition(const UShort_t layer);
  virtual UShort_t GetNChannels(const Sector &sector);
  virtual Bool_t IsInstrumented(const Channel &ch);
  virtual Channel FirstInstrumentedChannel(const Sector &sector);
  virtual Channel LastInstrumentedChannel(const Sector &sector);
  virtual Bool_t SectorExists(const Sector &sector);
  virtual Bool_t ChannelExists(const Channel &ch);
  virtual Channel GetFirstChannel(const Sector &sector);
  virtual Channel GetLastChannel(const Sector &sector);
  virtual Bool_t InFiducialArea(const Double_t position, const Sector &sector);
  virtual Bool_t InFiducialArea(const Double_t posX, const Double_t posY,
				const Sector &sector);
  virtual Double_t PositionChannel(const Channel &ch);
  virtual void PositionPad(const Channel &ch, Double_t &xMin, Double_t &xMax,
			   Double_t &yMin, Double_t &yMax);
  virtual std::pair<Double_t,Double_t> PositionEdgesChannel(const Channel &ch);
  virtual std::pair<Double_t,Double_t> GetLimitsChannels(const Sector &sector);
  virtual UShort_t GetID(const Double_t position, const Sector &sector);
  virtual UShort_t GetID(const Double_t xPos, const Double_t yPos,
			    const Sector &sector);
  virtual Bool_t AreNeighbours(const Channel &pad1, const Channel &pad2);
  virtual Bool_t OnStructuralSupport(const Double_t posX, const Double_t posY,
				     const UShort_t layer);

  static std::set<std::string> GetSupportedTypes();
  virtual std::vector<Double_t> GetSupportsWidth(const UShort_t layer);
  virtual std::vector<Double_t> GetSupportsY(const UShort_t layer);
  virtual std::pair<Double_t, Double_t> GetModuleLimitsX();
  virtual std::pair<Double_t, Double_t> GetModuleLimitsY();

  // Logical pads
  virtual Bool_t HasLogicalPad(const Double_t posX, const Double_t posY);
  virtual std::string GetLogicalPad(const Double_t posX, const Double_t posY);
  virtual Bool_t IsLogicalPad(const std::string &logicalPad);
  virtual std::set<std::string> GetAllLogicalPads(const double inc);  
  virtual std::set<std::string> GetAllLogicalPads();
  inline virtual std::pair<Sector,Sector> GetLogicalPadsReferenceSectors();
  inline virtual Logic GetPadLogic(){return padLogic;};
  virtual Int_t GetNPadRows(const UShort_t layer);
  virtual Int_t GetNPadColumns(const UShort_t layer);
  
  const std::string detectortype;
  

 private:

  // Helper functions
  void GenerateLogicalPads(const double inc);
  Double_t PositionFirstChannel(const Sector &sector);
  Double_t PositionSecondChannel(const Sector &sector);
  Double_t PositionLastChannel(const Sector &sector);
  std::string GetBoardKey(const Sector &sector);
  std::vector<Double_t> FlipBoardVector(const std::vector<Double_t> &v);
  Bool_t InTrapezoid(const Double_t posX, const Double_t posY,
		     const Double_t beginY, const Double_t H1,
		     const Double_t H2, const Double_t LB,
		     const Double_t angleDeg, const Double_t sideMargin=0);
  

  // Pad variables
  Logic padLogic;
  std::set<std::string> logicalPads;
  Double_t precisionLogicalPads=5;

  //===================================================
  // List of parameters
  // Info on parameters in NSW_modules_parameters.xlsx  
  // GENERAL
  const std::set<UShort_t>         layers = {1,2,3,4};
  Double_t                         globalPhiAngle=std::numeric_limits<Double_t>::quiet_NaN();
  std::map<std::string, Int_t>     nChannels;
  std::map<UShort_t, Double_t>     wirePlanesZPositions; // map of layer to wires planes zpos
  
  // CATHODE BOARDS
  std::map<std::string, Double_t>  yBeginBoard;
  std::map<std::string, Double_t>  boardH1;
  std::map<std::string, Double_t>  boardH2;
  std::map<std::string, Double_t>  boardLB;
  std::map<std::string, Double_t>  yDistIPtoLB;
  
  // PADS
  std::map<std::string, Int_t>     nPadRows;
  std::map<std::string, Double_t>  yBeginFirstRow;
  std::map<std::string, Double_t>  yEndFirstRow;
  std::map<std::string, Double_t>  yBeginLastRow;
  std::map<std::string, Double_t>  yEndLastRow;
  std::map<std::string, Double_t>  yHeightRow;
  std::map<std::string, std::vector<Double_t>>  phiLimits;
  std::map<std::string, std::vector<Double_t>>  offsetLimits;
  std::map<std::string, Double_t>  sideEdge;

  // STRIPS
  std::map<std::string, Double_t>  yBeginFirstStrip;
  std::map<std::string, Double_t>  yEndFirstStrip;
  std::map<std::string, Double_t>  yBeginLastStrip;
  std::map<std::string, Double_t>  yEndLastStrip;

  // WIRE
  std::map<std::string, Double_t>  xBeginFirstGroup;
  std::map<std::string, Double_t>  xEndFirstGroup;
  std::map<std::string, Double_t>  xBeginLastGroup;
  std::map<std::string, Double_t>  xEndLastGroup;

  // SUPPORTS
  Double_t buttonDiameter=std::numeric_limits<Double_t>::quiet_NaN();

  std::map<std::string, Double_t> yBeginGap;
  std::map<std::string, Double_t> gapH1;
  std::map<std::string, Double_t> gapH2;
  std::map<std::string, Double_t> gapLB;
  std::map<std::string, std::vector<Double_t>> buttonX;
  std::map<std::string, std::vector<Double_t>> buttonY;
  std::map<std::string, std::vector<Double_t>> supportsY;
  std::map<std::string, std::vector<Double_t>> supportsWidth;

  // XML utilities
  static std::exception invalidXML;
  static inline bool   xmlComp(const xmlNodePtr type, const std::string name);
  static inline bool   xmlComp(const xmlChar *key, const std::string name);
  static inline int    CInt(const xmlChar *key);
  static inline double CFlt(const xmlChar *key);
  static inline std::string CStr(const xmlChar *key);
  void ParseXML(const std::string &inFileName);
  static std::vector<Double_t> Parser(const xmlChar *key);
  static std::vector<Double_t> Parser(const std::string &a);
  static bool IsValidNumber(const xmlChar *key);
  static bool IsValidNumber(const std::string &s);
};

#endif
