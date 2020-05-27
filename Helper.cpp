#define Helper_cxx
#include "Helper.h"

using namespace std;

void getOtherLayers(UShort_t la, UShort_t lb, UShort_t* lc, UShort_t* ld) {
    switch(la) {
        case 1 : switch(lb) {
                     case 2 : *lc=3; *ld=4;
                              return;                        
                     case 3 : *lc=2; *ld=4;
                              return;
                     case 4 : *lc=2; *ld=3;
                              return;
                     default : throw runtime_error("Invalid layer number.");
                  }
        case 2 : switch(lb) {
                     case 3 : *lc=1; *ld=4;
                              return;
                     case 4 : *lc=1; *ld=3;
                              return;
                     default : throw runtime_error("Invalid layer number.");

                 }
        case 3 : switch(lb) {
                     case 4 : *lc=1; *ld=2;
                              return;
                     default : throw runtime_error("Invalid layer number.");
                 }
        default : throw runtime_error("Invalid layer number.");
    }
    return;
}

