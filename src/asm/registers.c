/** 
 * @file registers.c
 * @brief Static register definition
 */

#include <string.h>     // strcasecmp
#include "registers.h"

Register registers[NOREGS] = {
// general registers
    [ 0]   = { 0,  "ZERO"},
    [ 1]   = { 1,  "R1"  },
    [ 2]   = { 2,  "R2"  },
    [ 3]   = { 3,  "R3"  },
    [ 4]   = { 4,  "R4"  },
    [ 5]   = { 5,  "R5"  },
    [ 6]   = { 6,  "R6"  },
    [ 7]   = { 7,  "R7"  },
    [ 8]   = { 8,  "R8"  },
    [ 9]   = { 9,  "R9"  },
    [10]   = {10,  "R10" },
    [11]   = {11,  "R11" },
    [12]   = {12,  "R12" },
    [13]   = {13,  "R13" },
    [14]   = {14,  "R14" },
    [15]   = {15,  "R15" },
    [16]   = {16,  "R16" },
    [17]   = {17,  "R17" },
    [18]   = {18,  "R18" },
    [19]   = {19,  "R19" },
    [20]   = {20,  "R20" },
    [21]   = {21,  "R21" },
    [22]   = {22,  "R22" },
    [23]   = {23,  "R23" },
    [24]   = {24,  "R24" },
    [25]   = {25,  "R25" },
    [26]   = {26,  "R26" },
    [27]   = {27,  "R27" },
    [28]   = {28,  "R28" },
    [29]   = {29,  "R29" },
    [30]   = {30,  "R30" },
    [31]   = {31,  "R31" },
    [32]   = {32,  "R32" },
    
// special registers

    [PC  ] = {PC,   "PC"  },
    [DS  ] = {PC,   "DS"  },
    [HS  ] = {PC,   "HS"  },
    [HE  ] = {PC,   "HE"  },
    [SP  ] = {SP,   "SP"  },
    [FP  ] = {FP,   "FP"  },
    [IR  ] = {IR,   "IR"  },
    [STAT] = {STAT, "STAT"},
    [ERR ] = {ERR,  "ERR" },
    [HI  ] = {HI,   "HI"  },
    [LO  ] = {LO,   "LO"  },
    [CMPR] = {CMPR, "CMPR"}
};


Register* get_register_byname(const char* name)
{
    Register *r;
    int i = 0;
    while (i < NOREGS) {
        r = registers + i;
        if (r->name && (strcasecmp(r->name, name) == 0)) {
            return r;
        }
        i++;
    }

    return NULL;
}
