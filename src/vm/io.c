#include <stdio.h>
#include <stdlib.h>

#include "core.h"
#include "regcheck.h"
#include "memory.h"

int core_in  (void)
{
    printf("%s\n", __func__);
    return 0;
}

int core_out (void)
{
    State *state = core_getstate();
    
    int a = state->instruction[1];
    int n = state->instruction[2];
//     int p = state->instruction[3];
    
    CHECK_READ(a);
    CHECK_READ(n);
//     CHECK_READ(p);
    CHECK_WRITE(n);
    
    int address = state->registers[a].value;
    int length  = state->registers[n].value;
//     int port    = state->registers[p].value;
    
    uint8_t *mem = calloc(length+1, 1);
    if (mem_read(mem, address, length) == -1) {
        return -1;
    }
    
    /* don't care about the port now, 
       we send everything to stdout */
    printf("%s", mem);
    
    free(mem);
    
    return 0;
}
