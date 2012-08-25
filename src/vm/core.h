#ifndef CORE_H
#define CORE_H

#include <stdint.h> // uint8_t

/* Global data of the core */
extern int       running;
extern uint8_t   instruction[4];

void core_init(void);
void core_run_program(void);
void core_fetch(void);
void core_execute(void);


#endif
