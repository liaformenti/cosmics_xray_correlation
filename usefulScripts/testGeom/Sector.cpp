#define Sector_cxx
#include "Sector.h"


using namespace std;

// Electrode type
std::ostream  &operator<<(std::ostream& os, const ElectrodeType &type){
  switch (type){
  case fSTRIP:   os<<"S"; break;
  case fWIRE:    os<<"W"; break;
  case fPAD:     os<<"P"; break;
  case fOTHER:   os<<"O"; break;
  case fUNKNOWN: os<<"X"; break;
  default: throw logic_error("Invalid electrode type.");
  }

  return os;
};



// Channel
string Channel::CStr(const string &delimiter) const{
  stringstream ss;
  ss<<GetSector().CStr()<<delimiter<<electrode_ID;
  return ss.str();
}

Sector Channel::GetSector() const{
  return Sector(electrode_type, layer);
}

bool operator==(const Channel& lhs, const Channel& rhs){
  return (lhs.electrode_type==rhs.electrode_type) &&
    (lhs.layer==rhs.layer) &&
    (lhs.electrode_ID==rhs.electrode_ID);
}

bool operator!=(const Channel& lhs, const Channel& rhs){
  return !(lhs==rhs);
}

bool operator<(const Channel& lhs, const Channel& rhs){
  return (lhs.electrode_type<rhs.electrode_type) ||
    (lhs.electrode_type==rhs.electrode_type && lhs.layer<rhs.layer) ||
    (lhs.electrode_type==rhs.electrode_type && lhs.layer==rhs.layer && lhs.electrode_ID < rhs.electrode_ID);
}

bool operator>(const Channel& lhs, const Channel& rhs){
  return lhs!=rhs && !(lhs<rhs);
}

std::ostream  &operator<<(std::ostream& os, const Channel &ch){
  os<<"L"<<ch.layer<<ch.electrode_type<<":"<<ch.electrode_ID;
  return os;
};




// Sector
string Sector::CStr() const{
  stringstream ss;
  ss<<"L"<<layer<<electrode_type;
  return ss.str();
}


string Print(const Sector& sector){
  return sector.CStr();
}


bool operator==(const Sector& lhs, const Sector& rhs){
  return (lhs.electrode_type==rhs.electrode_type) &&
    (lhs.layer==rhs.layer);
}

bool operator!=(const Sector& lhs, const Sector& rhs){
  return !(lhs==rhs);
}

bool operator<(const Sector& lhs, const Sector& rhs){
  return (lhs.electrode_type<rhs.electrode_type) ||
    (lhs.electrode_type==rhs.electrode_type && lhs.layer<rhs.layer);
}

bool operator>(const Sector& lhs, const Sector& rhs){
  return lhs!=rhs && !(lhs<rhs);
}


ostream  &operator<<(ostream& os, const Sector &sector){
    os<<"L"<<sector.layer<<sector.electrode_type;
  return os;
};


// Converts a string into a sector
// 'inverse' of the "Print()" operation
Sector GetSector(string name){
  Sector s;
  s.layer = static_cast<UShort_t>(name[1]-'0');
  switch(name[2]){
  case('S'):
    s.electrode_type = fSTRIP; break;
  case('P'):
    s.electrode_type = fPAD; break;
  case('W'):
    s.electrode_type = fWIRE; break;
  case('X'):
    s.electrode_type = fUNKNOWN; break;
  case('O'):
    s.electrode_type = fOTHER; break;
  default:
    throw runtime_error("Sector name not recognized.");
  }

  return s;
}


  
