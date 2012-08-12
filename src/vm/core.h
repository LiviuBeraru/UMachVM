#ifndef CORE_H
#define CORE_H

#include <stddef.h>     // NULL
#include "core_state.h" // State

void core_init(void);
void core_run_program(void);
void core_fetch(void);
void core_execute(void);
void core_dump_regs(void); // this function does not belong here
State* core_getstate(void);


#endif
