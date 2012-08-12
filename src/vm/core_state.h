#ifndef CORE_STATE_H
#define CORE_STATE_H

#include <stdint.h>        // uint8_t
#include "core_register.h" // Register

typedef
struct core_state {
    int       running;
    Register  *registers;
    uint8_t   instruction[4];
}
State
;

#endif