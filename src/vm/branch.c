#include <stdio.h>
#include "core.h"
#include "branch.h"
#include "registers.h"

int core_be(void)
{
    State *state = core_getstate();
    if (state->registers[CMPR].value == 0) {
        return core_jmp();
    }
    return 0;
}

int core_bne(void)
{
    State *state = core_getstate();
    if (state->registers[CMPR].value != 0) {
        return core_jmp();
    }
    return 0;
}

int core_bl(void)
{
    State *state = core_getstate();
    if (state->registers[CMPR].value < 0) {
        return core_jmp();
    }
    return 0;
}

int core_ble (void)
{
    State *state = core_getstate();
    if (state->registers[CMPR].value <= 0) {
        return core_jmp();
    }
    return 0;
}

int core_bg  (void)
{
    State *state = core_getstate();
    if (state->registers[CMPR].value > 0) {
        return core_jmp();
    }
    return 0;
}

int core_bge (void)
{
    State *state = core_getstate();
    if (state->registers[CMPR].value >= 0) {
        return core_jmp();
    }
    return 0;
}

int core_jmp (void)
{
    State *state = core_getstate();
    
    /* extract the 3 bytes branch offset */
    int32_t offset = state->instruction[1];
    offset = offset << 8;
    offset = offset | state->instruction[2];
    offset = offset << 8;
    offset = offset | state->instruction[3];
    /* could also do some pointer magic instead of shifting */
    
    /* if the offset if negative, then the first bit 
       of the second byte should be one. if this is true, we set
       the whole first byte to one, in order to preserve the 
       sign bit */
    
    if (offset & 0x800000) {
        offset = offset | 0xFF000000;
    }
    
    /* the offset is the number of instructions to jump over
       because an instruction is 4 bytes long, we multiply the
       offset with 4 */
    offset = offset * 4;    
    state->registers[PC].value += offset;
    
    if (offset < 0) {
        state->registers[PC].value -= 8;
    }
    return 0;
}
