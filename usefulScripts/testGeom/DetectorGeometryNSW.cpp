
//=========================================================//
// Detector Geometry for NSW sTGC modules                  //
// Electrode number goes from 1 to N                       //
// See README.md for explanations on the coordinate system //
//=========================================================//


#define DetectorGeometryNSW_cxx
#include "DetectorGeometryNSW.h"

using namespace std;


//=========================================================================
// Constructor and XML parser
// Must specify detector type
// List of allowed detector types given by function: 'GetSupportedTypes()'
// The constructor will search for a file named geometry/detectortype.xml
// Throws exception if not found.
// Geometry parameters found in the XML files
// Full description of geometry parameters in the README
DetectorGeometryNSW::DetectorGeometryNSW(const string _detectortype)
  : detectortype(_detectortype){

  switch(detectortype.at(3)){
  case 'P': padLogic=fPIVOT; break;
  case 'C': padLogic=fCONFIRM; break;
  default: throw invalidParameters;
  }


  stripPitch = numeric_limits<Double_t>::quiet_NaN();
  wirePitch   = numeric_limits<Double_t>::quiet_NaN();

  // Validation (check if XML files exist)
  set<string> supportedTypes = GetSupportedTypes();
  if(supportedTypes.find(detectortype)==supportedTypes.end()){
    string message = "Detector type '"+detectortype+"' not supported by 'DetectorGeometryNSW'.";
    throw runtime_error(message);
  }


  // Read pad XML files
  string subtype=detectortype, type=detectortype.substr(0,3);
  ParseXML("geometry/"+subtype+".xml");
  ParseXML("geometry/"+type+".xml");
  GenerateLogicalPads(precisionLogicalPads);
}

void DetectorGeometryNSW::ParseXML(const string &inFileName){
  xmlDocPtr doc = xmlParseFile(inFileName.c_str());
  if(doc==NULL){
    cout<<"ERROR: Cannot read file: '"<<inFileName<<"'!"<<endl;
    throw runtime_error("Cannot read pad geometry XML file.");
  }
   
  xmlNodePtr root = xmlDocGetRootElement(doc);
  if(root==NULL){
    xmlFreeDoc(doc);
    throw invalidXML;
  }

  if(!xmlComp(root, "geometry")){
    xmlFreeDoc(doc);
    throw invalidXML;
  }

  for(xmlNodePtr it=root->xmlChildrenNode; it!=NULL; it=it->next){

    const string keyFull(CStr(it->name)); 
    const vector<string> keyVec = Tools::split(keyFull, '_');
    string key, keyM;
    if(keyVec.size()==1){
      key = keyVec.at(0);
    }
    else if(keyVec.size()==2){
      key    = keyVec.at(0);
      keyM = keyVec.at(1);
    }
    else throw invalidXML;
    
    xmlChar *val = xmlNodeListGetString(doc, it->xmlChildrenNode, 1);
    if(val!=NULL){
      if     (key=="nChannels")  nChannels[keyM]= CInt(val);
      else if(key=="nPadRows")  nPadRows[keyM]=CInt(val);
      else if(key=="yDistIPtoLB") yDistIPtoLB[keyM]=CFlt(val);
      else if(key=="yBeginBoard") yBeginBoard[keyM]=CFlt(val);
      else if(key=="boardH1") boardH1[keyM]=CFlt(val);
      else if(key=="boardH2") boardH2[keyM]=CFlt(val);
      else if(key=="boardLB") boardLB[keyM]=CFlt(val);
      else if(key=="yBeginFirstRow") yBeginFirstRow[keyM]=CFlt(val);
      else if(key=="yEndFirstRow") yEndFirstRow[keyM]=CFlt(val);
      else if(key=="yBeginLastRow") yBeginLastRow[keyM]=CFlt(val);
      else if(key=="yEndLastRow") yEndLastRow[keyM]=CFlt(val);
      else if(key=="yHeightRow") yHeightRow[keyM]=CFlt(val);
      else if(key=="phiLimits")	phiLimits[keyM]=Parser(val);
      else if(key=="offsetLimits") offsetLimits[keyM]=Parser(val);
      else if(key=="sideEdge") sideEdge[keyM]=CFlt(val);
      else if(key=="stripPitch") stripPitch=CFlt(val);
      else if(key=="wirePitch") wirePitch=CFlt(val);
      else if(key=="globalPhiAngle") globalPhiAngle=CFlt(val);
      else if(key=="yBeginFirstStrip") yBeginFirstStrip[keyM]=CFlt(val);
      else if(key=="yEndFirstStrip") yEndFirstStrip[keyM]=CFlt(val);
      else if(key=="yBeginLastStrip") yBeginLastStrip[keyM]=CFlt(val);
      else if(key=="yEndLastStrip") yEndLastStrip[keyM]=CFlt(val);

      else if(key=="xBeginFirstGroup") xBeginFirstGroup[keyM]=CFlt(val);
      else if(key=="xEndFirstGroup") xEndFirstGroup[keyM]=CFlt(val);
      else if(key=="xBeginLastGroup") xBeginLastGroup[keyM]=CFlt(val);
      else if(key=="xEndLastGroup") xEndLastGroup[keyM]=CFlt(val);

      else if(key=="buttonDiameter") buttonDiameter=CFlt(val);

      else if(key=="yBeginGap") yBeginGap[keyM]=CFlt(val);
      else if(key=="gapH1") gapH1[keyM]=CFlt(val);
      else if(key=="gapH2") gapH2[keyM]=CFlt(val);
      else if(key=="gapLB") gapLB[keyM]=CFlt(val);
      else if(key=="supportsWidth") supportsWidth[keyM]=Parser(val);
      else if(key=="supportsY") supportsY[keyM]=Parser(val);
      else if(key=="buttonX") buttonX[keyM]=Parser(val);
      else if(key=="buttonY") buttonY[keyM]=Parser(val);
      else if(key=="wirePlanesZPositions"){
	const vector<Double_t> zPositions = Parser(val);
	wirePlanesZPositions.clear();
	if(zPositions.size()!=4) throw runtime_error("Invalid XML file.");
	for(UShort_t l=1; l<=4; l++){
	  wirePlanesZPositions[l] = zPositions.at(l-1);
	}
      }
    }
    
    xmlFree(val);
  } // End all XML nodes

  xmlFreeDoc(doc);
}


UShort_t DetectorGeometryNSW::GetNLayers(){
  return 4;
}

set<Sector> DetectorGeometryNSW::GetSectors(){
  set<Sector> sectors;
  for(const auto &layer : layers){
    sectors.insert(Sector(fSTRIP, layer));
    sectors.insert(Sector(fWIRE, layer));
    sectors.insert(Sector(fPAD, layer));
  }

  return sectors;
}

// Same z position for all NSW modules
Double_t DetectorGeometryNSW::GetZPosition(const UShort_t layer){
  return wirePlanesZPositions.at(layer);
}


UShort_t DetectorGeometryNSW::
GetNChannels(const Sector &sector){
  if(!SectorExists(sector)) throw invalidParameters;
  return nChannels.at(GetBoardKey(sector));
}


// Assumes that all channels are instrumented for NSW
Bool_t DetectorGeometryNSW::
IsInstrumented(const Channel &ch){
  return ChannelExists(ch);
}

Channel DetectorGeometryNSW::
FirstInstrumentedChannel(const Sector &sector){
  return GetFirstChannel(sector);
}

Channel DetectorGeometryNSW::
LastInstrumentedChannel(const Sector &sector){
  return GetLastChannel(sector);
}



Bool_t DetectorGeometryNSW::
SectorExists(const Sector &sector){
  if(sector.layer<1 || sector.layer>4)
    return false;

  return (sector.electrode_type==fSTRIP || sector.electrode_type==fPAD
	  || sector.electrode_type==fWIRE);
}

Bool_t DetectorGeometryNSW::
ChannelExists(const Channel &ch){
  Sector sector = ch.GetSector();
  if(!SectorExists(sector))
    return false;

  UShort_t id = ch.electrode_ID;
  UShort_t idFirst=GetFirstChannel(sector).electrode_ID;
  UShort_t idLast=GetLastChannel(sector).electrode_ID;
  
  return (id>=idFirst && id<=idLast);
}

Channel DetectorGeometryNSW::
GetFirstChannel(const Sector &sector){
  if(!SectorExists(sector)) throw invalidParameters;
  return Channel(sector.electrode_type, sector.layer, 1);
}

Channel DetectorGeometryNSW::
GetLastChannel(const Sector &sector){
  if(!SectorExists(sector)) throw invalidParameters;
  return Channel(sector.electrode_type, sector.layer, GetNChannels(sector));
}



// This function only makes sense for wires and strips
// Must check that channel exists before calling the function
Double_t DetectorGeometryNSW::
PositionChannel(const Channel &ch){
  if(!ChannelExists(ch)) throw invalidParameters;

  const Sector sector = ch.GetSector();
  const ElectrodeType type = sector.electrode_type;
  
  if(type==fSTRIP || type==fWIRE){
    if(ch==GetFirstChannel(sector))
      return PositionFirstChannel(sector);
    else if(ch==GetLastChannel(sector))
      return PositionLastChannel(sector);
    else{
      const UShort_t ID = ch.electrode_ID;
      const ElectrodeType type = sector.electrode_type;
      return PositionSecondChannel(sector) + GetPitch(type)*(ID-2);
    }
  }
  else throw invalidParameters;  
}

void DetectorGeometryNSW::
PositionPad(const Channel &ch, Double_t &xMin, Double_t &xMax,
	    Double_t &yMin, Double_t &yMax){
  throw runtime_error("Cannot call function 'PositionPad' for NSW modules.");
}



pair<Double_t,Double_t> DetectorGeometryNSW::
PositionEdgesChannel(const Channel &ch){
  if(!ChannelExists(ch))  throw invalidParameters;

  const ElectrodeType type = ch.electrode_type;
  const UShort_t ID = ch.electrode_ID;
  const Sector sector = ch.GetSector();

  pair<Double_t,Double_t> edges;
  const string key = GetBoardKey(sector);

  // Get edges
  if(ID==1 && type==fSTRIP){
    edges.first  = yBeginFirstStrip.at(key);
    edges.second = yEndFirstStrip.at(key);
  }
  else if(ID==1 && type==fWIRE){
    edges.first  = xBeginFirstGroup.at(key);
    edges.second = xEndFirstGroup.at(key);
  }
  else if(ID==GetNChannels(sector) && type==fSTRIP){
    edges.first  = yBeginLastStrip.at(key);
    edges.second = yEndLastStrip.at(key);
  }
  else if(ID==GetNChannels(sector) && type==fWIRE){
    edges.first  = xBeginLastGroup.at(key);
    edges.second = xEndLastGroup.at(key);
  }
  else if(type==fSTRIP || type==fWIRE){
    edges.first = PositionChannel(ch) - 0.5*GetPitch(type);
    edges.second = PositionChannel(ch) + 0.5*GetPitch(type);
  }
  else throw invalidParameters;
  
  return edges;
}


// Returns the edges of the first and last channels
pair<Double_t,Double_t> DetectorGeometryNSW::
GetLimitsChannels(const Sector &sector){
  if(!SectorExists(sector)) throw invalidParameters;

  const ElectrodeType type = sector.electrode_type;
  if(type==fSTRIP){
    Double_t min = PositionEdgesChannel(GetFirstChannel(sector)).first;
    Double_t max = PositionEdgesChannel(GetLastChannel(sector)).second;
    return make_pair(min, max);
  }
  else if(type==fWIRE){
    Double_t min = PositionEdgesChannel(GetFirstChannel(sector)).first;
    Double_t max = PositionEdgesChannel(GetLastChannel(sector)).second;
    return make_pair(min, max);
  }
  else throw invalidParameters; 
}


// If this function returns false, the hit position does not
// correspond to any valid electrode ID. Function only valid
// for strips and wires because 1D. The 2D version of the
// function works for pads.
Bool_t DetectorGeometryNSW::
InFiducialArea(const Double_t position, const Sector &sector){
  pair<Double_t, Double_t> limits = GetLimitsChannels(sector);
  return (position>=limits.first && position<=limits.second);
}


// 2D version of InFiducialArea. Essential for pads.
// Important for strip and wire of NSW modules that have
// a trapezoid shape.
Bool_t DetectorGeometryNSW::
InFiducialArea(const Double_t posX, const Double_t posY, const Sector &sector){
  if(!SectorExists(sector)) throw invalidParameters;
  const ElectrodeType type = sector.electrode_type;

  if(type==fSTRIP){
    if(!InFiducialArea(posY, sector))
      return false;
    else{
      string key = "GV"+Tools::CStr(sector.layer);
      return InTrapezoid(posX, posY, yBeginGap.at(key), gapH1.at(key),
			 gapH2.at(key), gapLB.at(key), globalPhiAngle);
    }
  }
  else if(type==fWIRE){
    if(!InFiducialArea(posX, sector))
      return false;
    else{
      string key = "GV"+Tools::CStr(sector.layer);
      return InTrapezoid(posX, posY, yBeginGap.at(key), gapH1.at(key),
			 gapH2.at(key), gapLB.at(key), globalPhiAngle);
    }
  }
  else if(type==fPAD){
    string key = GetBoardKey(sector);
    if(posY<yBeginFirstRow.at(key) || posY>yEndLastRow.at(key)){
      return false;
    }
    else{
      return InTrapezoid(posX, posY, yBeginBoard.at(key), boardH1.at(key),
			 boardH2.at(key), boardLB.at(key), globalPhiAngle,
			 sideEdge.at(key));
    }
  }
  else throw invalidParameters;
   
}
	 
//============================================================
// GetID functions assume the detector is in the fiducial area
// May throw an exception if not the case
//============================================================

// Function for strips and wires (1D)
// ID is undefined if not in fiducial area hence throwing an exception.
UShort_t DetectorGeometryNSW::
GetID(const Double_t position, const Sector &sector){
  if(!InFiducialArea(position, sector)) throw invalidParameters;

  ElectrodeType t = sector.electrode_type;
  string key = GetBoardKey(sector);
  Double_t pitch = GetPitch(t);
  
  if(t==fSTRIP){
    if(position<=yEndFirstStrip.at(key))
      return GetFirstChannel(sector).electrode_ID;
    else if(position>yBeginLastStrip.at(key))
      return GetLastChannel(sector).electrode_ID;
    else
      return ceil((position-(PositionSecondChannel(sector)-pitch/2.))/pitch)+1;
  }
  else if(t==fWIRE){
    if(position<=xEndFirstGroup.at(key))
      return GetFirstChannel(sector).electrode_ID;
    else if(position>xBeginLastGroup.at(key))
      return GetLastChannel(sector).electrode_ID;
    else
      return ceil((position-(PositionSecondChannel(sector)-pitch/2.))/pitch)+1;
  }
  else throw invalidParameters;
}


// GetID function in 2D is only for pads
// Pad columns and rows numbered from 0 to N-1
// Final pad numbered from 1 to N
// (is reversing Alam's numbering convention as of 2019-03-14)
UShort_t DetectorGeometryNSW::
GetID(const Double_t xPos, const Double_t yPos, const Sector &sector){
  auto l = sector.layer;
  auto t = sector.electrode_type;
  if(t!=fPAD || !InFiducialArea(xPos, yPos, sector))
    throw invalidParameters;

  const string key = GetBoardKey(sector);

  // Find pad row
  Int_t row=-1;
  if(yPos <= yEndFirstRow.at(key))
    row = 0;
  else if(yPos > yBeginLastRow.at(key))
    row = nPadRows.at(key)-1;
  else
    row = ceil( (yPos - yEndFirstRow.at(key)) / yHeightRow.at(key) );

  // Manage board flip
  // Depends if board is flipped (for L1 and L3)
  vector<Double_t> flipPhiLimits, flipOffsetLimits;
  if(l==2 || l==4){
    flipPhiLimits    = phiLimits.at(key);
    flipOffsetLimits = offsetLimits.at(key);
  }
  else if(l==1 || l==3){
    flipPhiLimits    = FlipBoardVector(phiLimits.at(key));
    flipOffsetLimits = FlipBoardVector(offsetLimits.at(key));
  }

  // Find pad columns
  Int_t nColumns = flipPhiLimits.size() + 1;
  Int_t column = nColumns-1; // default value  
  for(Int_t i=0; i<flipPhiLimits.size(); i++){
    Double_t angRad = Tools::DegToRad(flipPhiLimits.at(i));
    Double_t slope  = tan(angRad); // slope of x(y)
    Double_t xLimit = slope*(yPos+yDistIPtoLB.at(key)-boardH1.at(key)-boardH2.at(key));
    Double_t xOffset = flipOffsetLimits.at(i) / cos(angRad);
    xLimit += xOffset;

    if(xLimit>xPos){
      column = i;
      break;
    }
  }

  // Return final pad
  return row*nColumns + column + 1;
}



// Check if 2 channels are neighbours
// Trivial for wire and strip, but not pads
// If channels are the same: returns 'true'.
Bool_t DetectorGeometryNSW::
AreNeighbours(const Channel &ch1, const Channel &ch2){
  if(!ChannelExists(ch1) || !ChannelExists(ch2))
    throw invalidParameters; 

  if((ch1.layer!=ch2.layer) || (ch1.electrode_type!=ch2.electrode_type))
    return false;
  
  const Int_t ID1 = ch1.electrode_ID;
  const Int_t ID2 = ch2.electrode_ID;
    
  if(ID1==ID2)
    return true;

  if(ch1.electrode_type!=fPAD)
    return fabs(ID1-ID2)<=1;

  const string key = GetBoardKey(ch1.GetSector());
  const Int_t nCols = phiLimits.at(key).size()+1;

  const Int_t row1    = (ID1-1) / nCols;
  const Int_t row2    = (ID2-1) / nCols;
  const Int_t col1    = (ID1-1) % nCols;
  const Int_t col2    = (ID2-1) % nCols;

  return (fabs(row1-row2)<=1 && fabs(col1-col2)<=1);
}


Double_t DetectorGeometryNSW::
PositionFirstChannel(const Sector &sector){
  const string key = GetBoardKey(sector);
  if(sector.electrode_type==fSTRIP)
    return 0.5*(yBeginFirstStrip.at(key)+yEndFirstStrip.at(key));
  else if(sector.electrode_type==fWIRE)
    return 0.5*(xBeginFirstGroup.at(key)+xEndFirstGroup.at(key));
  else throw invalidParameters;
}

Double_t DetectorGeometryNSW::
PositionSecondChannel(const Sector &sector){
  const string key = GetBoardKey(sector);
  const Double_t pitch = GetPitch(sector.electrode_type);
  if(sector.electrode_type==fSTRIP)
    return yEndFirstStrip.at(key) + 0.5*pitch;
  else if(sector.electrode_type==fWIRE)
    return xEndFirstGroup.at(key) + 0.5*pitch;
  throw invalidParameters;
}

Double_t DetectorGeometryNSW::
PositionLastChannel(const Sector &sector){
  const string key = GetBoardKey(sector);
  if(sector.electrode_type==fSTRIP)
    return 0.5*(yBeginLastStrip.at(key)+yEndLastStrip.at(key));
  else if(sector.electrode_type==fWIRE)
    return 0.5*(xBeginLastGroup.at(key)+xEndLastGroup.at(key));
  else throw invalidParameters;
}

// Returns an empty vector if information on supports is unknown
vector<Double_t> DetectorGeometryNSW::GetSupportsWidth(const UShort_t layer){  
  if(layer<1 || layer>4) throw invalidParameters;
  const string key = "GV"+Tools::CStr(layer);
  auto it = supportsWidth.find(key);
  if(it==supportsWidth.end()){
    return vector<Double_t>(0);
  }
  else{
    return it->second;
  }
}

// Returns an empty vector if information on supports is unknown
vector<Double_t> DetectorGeometryNSW::GetSupportsY(const UShort_t layer){  
  if(layer<1 || layer>4) throw invalidParameters;
  const string key = "GV"+Tools::CStr(layer);
  auto it = supportsY.find(key);
  if(it==supportsY.end()){
    return vector<Double_t>(0);
  }
  else{
    return it->second;
  }
}


// Returns "true" if (posX,posY) coordinates for layer fall
// on a structural support or outside of the gas volume
Bool_t DetectorGeometryNSW::
OnStructuralSupport(const Double_t posX, const Double_t posY,
		    const UShort_t layer){

  const string key = "GV"+Tools::CStr(layer);
  
  // Check if outside gas volume
  if(!InTrapezoid(posX, posY, yBeginGap.at(key), gapH1.at(key), gapH2.at(key),
		  gapLB.at(key), globalPhiAngle)){
    return true;
  }

  // Check buttons
  if(buttonX.find(key)!=buttonX.end()){
    const Int_t nButtons = buttonX.at(key).size();
    if(nButtons!=buttonY.size()) throw runtime_error("Invalid button data.");
    for(Int_t i=0; i<nButtons; i++){
      Double_t dx = posX - buttonX.at(key).at(i);
      Double_t dy = posY - buttonY.at(key).at(i);
      Double_t r2 = dx*dx + dy*dy;
      if(r2 < pow(0.5*buttonDiameter,2))
	return true;
    }
  }

  // Check supports
  if(supportsY.find(key)!=supportsY.end()){
    const Int_t nSupports = supportsY.at(key).size();
    if(nSupports != supportsWidth.at(key).size()) throw runtime_error("Invalid supports data.");
    
    for(Int_t i=0; i<nSupports; i++){
      Double_t bottomSupport = supportsY.at(key).at(i);
      Double_t topSupport = bottomSupport+supportsWidth.at(key).at(i);
      if(posY>=bottomSupport && posY<=topSupport){
	return true;
      }
    }
  }

  return false;
}

string DetectorGeometryNSW::GetBoardKey(const Sector &sector){
  if(sector.electrode_type==fSTRIP){
    switch(sector.layer){
    case 1: case 3: return "K13";
    case 2: case 4: return "K24";
    }
  }
  else if(sector.electrode_type==fPAD){
    switch(sector.layer){
    case 1: case 2: return "H12";
    case 3: case 4: return "H34";
    }
  }
  else if(sector.electrode_type==fWIRE){
    return "W"+Tools::CStr(sector.layer);
  }

  throw invalidParameters;
}


vector<Double_t> DetectorGeometryNSW::FlipBoardVector(const vector<Double_t> &v){
  vector<Double_t> vOut;
  for(Int_t i=v.size()-1; i>=0; i--)
    vOut.push_back(-1*v.at(i));

  return vOut;
}

// Assumes the trapezoid is symmetric
Bool_t DetectorGeometryNSW::
InTrapezoid(const Double_t posX, const Double_t posY, const Double_t beginY,
	    const Double_t H1, const Double_t H2, const Double_t LB,
	    const Double_t angleDeg, const Double_t sideMargin){


  // If in shoulder
  if(posY>=(beginY+H1) && posY<=(beginY+H1+H2)){
    Double_t xSide = 0.5*LB - sideMargin;
    return fabs(posX)<=xSide;
  }

  // if lower
  else if(posY>=(beginY) && posY<(beginY+H1)){
    Double_t xSide = 0.5*LB - sideMargin/cos(Tools::DegToRad(0.5*angleDeg));
    xSide -= fabs(tan(Tools::DegToRad(0.5*angleDeg))) * (beginY+H1-posY);
    return fabs(posX)<=xSide;
  }
  else
    return false;
}

// Modules limits should be independant on the layer
// In general, strip boards are larger and pad boards are higher.
pair<Double_t, Double_t> DetectorGeometryNSW::GetModuleLimitsX(){
  return make_pair(-0.5*boardLB.at("K13"), 0.5*boardLB.at("K13"));
}
pair<Double_t, Double_t> DetectorGeometryNSW::GetModuleLimitsY(){
  return make_pair(yBeginBoard.at("H12"),
		   boardH1.at("H12")+boardH2.at("H12"));
}


// Look into geometry directory for XML files.
set<string> DetectorGeometryNSW::GetSupportedTypes(){

  set<string> types, typeBase;
  const string dirName("geometry");
  DIR *dir;
  struct dirent *ent;
  if ((dir = opendir(dirName.c_str())) != NULL) {
    
    // Get all 4-letter types
    while ((ent = readdir (dir)) != NULL) {
      string name(ent->d_name);
      size_t len=name.size();
      if(len>=7 && name[0]=='Q' && name.substr(len-4,4)==".xml"){
	if(len==8){
	  types.insert(name.substr(0,4));
	}
	else if(len==7){
	  typeBase.insert(name.substr(0,3));
	}
      }
    }
    closedir (dir);
    delete ent;
  
    // Check that all 4-letter types are associated with 3-letter type.
    for(auto it=types.begin(); it!=types.end();){

      string curBase = (*it).substr(0,3);
      if(typeBase.find(curBase)==typeBase.end()){
	cout<<"WARNING: Cannot find file geometry/"<<curBase<<".xml for type "<<(*it)<<"."<<endl;
	it = types.erase(it);

      }
      else{
	it++;
      }
    }

  }
  else{
    cout<<"WARNING: Unable to directory 'geometry'."<<endl;
  }
  
  return types;
}




// XML utilities //
exception DetectorGeometryNSW::
invalidXML = runtime_error("Invalid geometry XML file.");

bool DetectorGeometryNSW::xmlComp(const xmlNodePtr type, const std::string name){
  return xmlStrcmp(type->name, (const xmlChar *)name.c_str())==0;
}


bool DetectorGeometryNSW::xmlComp(const xmlChar *key, const std::string name){
  return !xmlStrcmp(key, (const xmlChar *) name.c_str());
}


int DetectorGeometryNSW::CInt(const xmlChar *key){
  if(!IsValidNumber(key)){
    cout<<"'"<<key<<"'"<<endl;
    return 0;
  }

  return atoi((char*) key);
}


double DetectorGeometryNSW::CFlt(const xmlChar *key){

  if(!IsValidNumber(key)){
    return numeric_limits<Double_t>::quiet_NaN();
  }

  return atof((char*) key);
}

string DetectorGeometryNSW::CStr(const xmlChar *key){
  string s;
  for(int i=0; i<xmlStrlen(key); i++){
    s.push_back(static_cast<char>(static_cast<int>(key[i])));
  }
  
  return s;
}
 
vector<Double_t> DetectorGeometryNSW::Parser(const xmlChar *key){
  return Parser(CStr(key));
}

vector<Double_t> DetectorGeometryNSW::Parser(const string &a){
  vector<Double_t> res(0);
  if(a.size()==0 || !IsValidNumber(a)){
    return res;
  }

  vector<string> aVec = Tools::split(a, ',');
  for(const auto &elem : aVec){
    res.push_back(atof(elem.c_str()));
  }

  return res;
}

// Check if the XML entry if a valid number (float or integer)
// Should only contain numbers (from '0' to '9'), comma (',') or dot ('.')
bool DetectorGeometryNSW::IsValidNumber(const xmlChar *key){
  return IsValidNumber(CStr(key));
}

bool DetectorGeometryNSW::IsValidNumber(const std::string &s){
  if(s.size()==0)
    return false;

  int nWhiteSpaces=0;
  for(const char c : s){
    if(c>='0' && c<='9') 
      continue;
    else if(c==',' || c=='.' || c=='-' || c=='E' || c=='+' || c=='e') 
      continue;
    else if(c==' ' || c=='\t') 
      nWhiteSpaces++;
    else return false;
  }
  
  if(nWhiteSpaces==s.size()){
    return false;
  }
  
  return true;
}


// LOGICAL PAD METHODS
// Logical pads are defined by the pad pattern of L1 and L3 for pivot
// modules and L1 and L2 for confirm modules.

// Tell whether this coordinate is located over a logical pad
Bool_t DetectorGeometryNSW::
HasLogicalPad(const Double_t posX, const Double_t posY){
  pair<Sector, Sector> refS = GetLogicalPadsReferenceSectors();
  return InFiducialArea(posX, posY, refS.first)
    && InFiducialArea(posX, posY, refS.second);
}

string DetectorGeometryNSW::
GetLogicalPad(const Double_t posX, const Double_t posY){
  pair<Sector, Sector> refS = GetLogicalPadsReferenceSectors();
  int L1 = refS.first.layer, L2=refS.second.layer;
  int ID1 = GetID(posX, posY, refS.first), ID2 = GetID(posX, posY, refS.second);
  return "L"+to_string(L1)+"ID"+to_string(ID1)+"_"+
    "L"+to_string(L2)+"ID"+to_string(ID2);
}

Bool_t DetectorGeometryNSW::IsLogicalPad(const string &logicalPad){
  return (logicalPads.find(logicalPad)!=logicalPads.end());
}

set<string> DetectorGeometryNSW::GetAllLogicalPads(const double inc){
  return logicalPads;
}

void DetectorGeometryNSW::GenerateLogicalPads(const double inc){
  precisionLogicalPads=inc;
  pair<double,double> limitsX=GetModuleLimitsX(), limitsY=GetModuleLimitsY();
  double posX=limitsX.first, posY=0;
  logicalPads.clear();
  while(posX<limitsX.second){
    posY=limitsY.first;
    while(posY<limitsY.second){
      if(HasLogicalPad(posX, posY)){
	logicalPads.insert(GetLogicalPad(posX, posY));
      }
      posY+=inc;
    }
    posX+=inc;
  }
}


set<string> DetectorGeometryNSW::GetAllLogicalPads(){
  return GetAllLogicalPads(5.);
}

pair<Sector,Sector> DetectorGeometryNSW::
GetLogicalPadsReferenceSectors(){
  if(padLogic==fPIVOT)
    return make_pair(Sector(fPAD, 1), Sector(fPAD, 3));
  else if(padLogic==fCONFIRM)
    return make_pair(Sector(fPAD, 1), Sector(fPAD, 4));
  else throw invalidParameters;
}


Int_t DetectorGeometryNSW::GetNPadRows(const UShort_t layer){
  return nPadRows.at(GetBoardKey(Sector(fPAD, layer)));
}


Int_t DetectorGeometryNSW::GetNPadColumns(const UShort_t layer){
  return (phiLimits.at(GetBoardKey(Sector(fPAD, layer))).size()+1);
}
