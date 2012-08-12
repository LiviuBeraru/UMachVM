#include "core.h"
#include "regcheck.h"
#include "memory.h"

#include <stdio.h>

int core_set(void)
{
    State *state = core_getstate();
    int reg = state->instruction[1];
    CHECK_WRITE(reg);

    int val = state->instruction[2];
    val = (val << 8) | state->instruction[3];

    state->registers[reg].value = val;

    return 0;
}

int core_cp(void)
{
    State *state = core_getstate();
    int dest = state->instruction[1];
    int src = state->instruction[2];

    CHECK_READ(src);
    CHECK_WRITE(dest);
    state->registers[dest].value = state->registers[src].value;

    return 0;
}

int core_lb(void)
{
    State *state = core_getstate();;
    int dest = state->instruction[1];
    int src = state->instruction[2];
    
    CHECK_WRITE(dest);
    CHECK_READ(src);
    
    /* Register numbers are ok. 
      Now call mem_read from the memory module.
      This call will trigger the appropriate interrupts 
      if reading fails. */
    
    int adress = state->registers[src].value;
    int32_t oldvalue = state->registers[dest].value;
    oldvalue = oldvalue & 0xFFFFFF00; // unset last byte
    
    uint8_t byte = 0;
    if (mem_read(&byte, adress, 1) != -1) {
        state->registers[dest].value = oldvalue | byte;
        return 0;
    } else {
        return -1;
    }
}

int core_lw(void)
{
    printf("%s\n", __func__);
    return 0;
}

int core_sb(void)
{
    State *state = core_getstate();;
    int src  = state->instruction[1];
    int dest = state->instruction[2];
    
    CHECK_READ(src);
    CHECK_READ(dest);
    
    int address = state->registers[dest].value;
    uint8_t value = state->registers[src].value & 0xFF;
    
    if (mem_write(&value, address, 1) == -1) {
        return -1;
    } else {
        return 0;
    }
}

int core_sw(void)
{
    printf("%s\n", __func__);
    return 0;
}

int core_push(void)
{
    printf("%s\n", __func__);
    return 0;
}

int core_pop(void)
{
    printf("%s\n", __func__);
    return 0;
}

