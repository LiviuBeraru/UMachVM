#ifndef REGCHECK_H
#define REGCHECK_H

#include "logmsg.h"
#include "interrupts.h"
#include "system.h" 


int reg_noread(int regno);
int reg_nowrite(int regno);


#define CHECK_READ(r) \
    if (reg_noread(r)) {\
    logmsg(LOG_ERR, "Cannot read from register %d", r);\
        interrupt(INT_INVALID_REG);\
        return -1;\
    }

#define CHECK_WRITE(r) \
    if (reg_nowrite(r)) {\
    logmsg(LOG_ERR, "Cannot write to register %d", r);\
        interrupt(INT_INVALID_REG);\
        return -1;\
    }

#endif