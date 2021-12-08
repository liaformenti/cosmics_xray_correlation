#ifndef Sector_h
#define Sector_h

// C++ includes
#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>

// ROOT includes
#include <Rtypes.h>

// My includes
// ...


// ElectrodeType
enum ElectrodeType : Int_t {fSTRIP=0, fPAD=1, fWIRE=2, fOTHER=3, fUNKNOWN=4};
std::ostream  &operator<<(std::ostream& os, const ElectrodeType &type);

// Sector
struct Sector{
  Sector(){};
Sector(ElectrodeType eType, UShort_t l) : electrode_type(eType), layer(l){};

  ElectrodeType electrode_type;
  UShort_t layer;

  std::string CStr() const;
};

// Operators for boolean comparison
// Required for use in map
bool operator==(const Sector& lhs, const Sector& rhs);
bool operator!=(const Sector& lhs, const Sector& rhs);
bool operator<(const Sector& lhs, const Sector& rhs);
bool operator>(const Sector& lhs, const Sector& rhs);
std::ostream &operator<<(std::ostream& os, const Sector &sector);
std::string Print(const Sector &sector);
Sector GetSector(std::string name);



// Channel
struct Channel{

  Channel(){};
Channel(ElectrodeType eType, UShort_t l, UShort_t id) :
  electrode_type(eType), layer(l), electrode_ID(id){};

  ElectrodeType electrode_type;
  UShort_t layer;
  UShort_t electrode_ID;

  std::string CStr(const std::string &delimiter=":") const;
  Sector GetSector() const;
};


// Operators for boolean comparison
// Required for use in map
bool operator==(const Channel& lhs, const Channel& rhs);
bool operator!=(const Channel& lhs, const Channel& rhs);
bool operator<(const Channel& lhs, const Channel& rhs);
bool operator>(const Channel& lhs, const Channel& rhs);
std::ostream &operator<<(std::ostream& os, const Channel &ch);









#endif
