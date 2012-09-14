/** 
 * @file registers.c
 * @brief Static register definition
 * 
 * This file defines a static table of register structures.
 * The motivation for this is that the register list is static 
 * (does not change), so we define it here instead of 
 * initialising it at runtime.
 * This table definition is analogue to the command definition in command.c.
 */

#include <stddef.h>     // NULL
#include <string.h>     // strcmp
#include "registers.h"
#include "logmsg.h"
#include "system.h"     // interrupt
#include "interrupts.h" // interrupt numbers


Register registers[NOREGS] = {
// general registers
    [ 0]   = { 0, 0,   REG_READ, "ZERO"},
    [ 1]   = { 1, 0,   REG_RW,   "R1"  },
    [ 2]   = { 2, 0,   REG_RW,   "R2"  },
    [ 3]   = { 3, 0,   REG_RW,   "R3"  },
    [ 4]   = { 4, 0,   REG_RW,   "R4"  },
    [ 5]   = { 5, 0,   REG_RW,   "R5"  },
    [ 6]   = { 6, 0,   REG_RW,   "R6"  },
    [ 7]   = { 7, 0,   REG_RW,   "R7"  },
    [ 8]   = { 8, 0,   REG_RW,   "R8"  },
    [ 9]   = { 9, 0,   REG_RW,   "R9"  },
    [10]   = {10, 0,   REG_RW,   "R10" },
    [11]   = {11, 0,   REG_RW,   "R11" },
    [12]   = {12, 0,   REG_RW,   "R12" },
    [13]   = {13, 0,   REG_RW,   "R13" },
    [14]   = {14, 0,   REG_RW,   "R14" },
    [15]   = {15, 0,   REG_RW,   "R15" },
    [16]   = {16, 0,   REG_RW,   "R16" },
    [17]   = {17, 0,   REG_RW,   "R17" },
    [18]   = {18, 0,   REG_RW,   "R18" },
    [19]   = {19, 0,   REG_RW,   "R19" },
    [20]   = {20, 0,   REG_RW,   "R20" },
    [21]   = {21, 0,   REG_RW,   "R21" },
    [22]   = {22, 0,   REG_RW,   "R22" },
    [23]   = {23, 0,   REG_RW,   "R23" },
    [24]   = {24, 0,   REG_RW,   "R24" },
    [25]   = {25, 0,   REG_RW,   "R25" },
    [26]   = {26, 0,   REG_RW,   "R26" },
    [27]   = {27, 0,   REG_RW,   "R27" },
    [28]   = {28, 0,   REG_RW,   "R28" },
    [29]   = {29, 0,   REG_RW,   "R29" },
    [30]   = {30, 0,   REG_RW,   "R30" },
    [31]   = {31, 0,   REG_RW,   "R31" },
    [32]   = {32, 0,   REG_RW,   "R32" },
    
// special registers

    [PC  ] = {PC,   256, REG_READ, "PC"  },
    [DS  ] = {PC,   0,   REG_READ, "DS"  },
    [HS  ] = {PC,   0,   REG_READ, "HS"  },
    [HE  ] = {PC,   0,   REG_RW,   "HE"  },
    [SP  ] = {SP,   0,   REG_RW,   "SP"  },
    [FP  ] = {FP,   0,   REG_RW,   "FP"  },
    [IR  ] = {IR,   0,   REG_READ, "IR"  },
    [STAT] = {STAT, 0,   REG_RW,   "STAT"},
    [ERR ] = {ERR,  0,   REG_RW,   "ERR" },
    [HI  ] = {HI,   0,   REG_READ, "HI"  },
    [LO  ] = {LO,   0,   REG_READ, "LO"  },
    [CMPR] = {CMPR, 0,   REG_READ, "CMPR"}
};

int read_register(int regno, int32_t* dest)
{
    if (regno < 0 || regno >= NOREGS) {
        logmsg(LOG_ERR, "Invalid register number: %d", regno);
        interrupt(INT_INVALID_REG);
        return -1;
    }
    
    if (registers[regno].mode & REG_READ) {
        *dest = registers[regno].value;
        return 0;
    } else {
        logmsg(LOG_ERR, "Register <%s> is not readable", 
               registers[regno].name);
        interrupt(INT_INVALID_REG);
        return -1;
    }
}

int read_registeru(int regno, uint32_t* dest)
{
    return read_register(regno, (int32_t*) dest);
}


int write_register(int regno, int32_t value)
{
    if (regno < 0 || regno >= NOREGS) {
        logmsg(LOG_ERR, "Invalid register number: %d", regno);
        interrupt(INT_INVALID_REG);
        return -1;
    }
    
    if (registers[regno].mode & REG_WRITE) {
        registers[regno].value = value;
        return 0;
    } else {
        logmsg(LOG_ERR, "Register <%s> is not writable", 
               registers[regno].name);
        interrupt(INT_INVALID_REG);
        return -1;
    }
}

int write_registeru(int regno, uint32_t value)
{
    return write_register(regno, (int32_t) value);
}

Register* get_register(int regno)
{
    if (regno < 0 || regno >= NOREGS) {
        logmsg(LOG_ERR, "Invalid register number %d", regno);
        interrupt(INT_INVALID_REG);
        return NULL;
    }
    
    Register *r = &(registers[regno]);
    if (r->name) {
        return r;
    } else {
        logmsg(LOG_ERR, "Invalid register number %d", regno);
        interrupt(INT_INVALID_REG);
        return NULL;
    }
}

Register* get_register_byname(const char* name)
{
    Register *r = NULL;
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
