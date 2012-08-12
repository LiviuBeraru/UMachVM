#include <stdio.h>
#include "core.h"
#include "regcheck.h"
#include "registers.h"

int core_cmp  (void)
{
    State *state = core_getstate();
    /* extract the argument */
    int a = state->instruction[1];
    int b = state->instruction[2];
    
    /* check the read pemissions of the 
     * specified registers */
    CHECK_READ(a);
    CHECK_READ(b);
    
    /* extract the register values */
    a = state->registers[a].value;
    b = state->registers[b].value;
    
    
    /* we don't check the write permissions
     of the CMPR register */
    if (a == b) {
        state->registers[CMPR].value =  0;
    } else if (a < b) {
        state->registers[CMPR].value = -1;
    } else {
        state->registers[CMPR].value =  1;
    }
    
    return 0;
}

int core_cmpu (void)
{
    printf("%s\n", __func__);
    return 0;
}

int core_cmpi (void)
{
    printf("%s\n", __func__);
    return 0;
}
