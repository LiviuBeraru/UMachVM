#include "core.h"
#include "branch.h"
#include "registers.h"

int core_be(void)
{
    if (registers[CMPR].value == 0) {
        return core_jmp();
    }
    return 0;
}

int core_bne(void)
{
    if (registers[CMPR].value != 0) {
        return core_jmp();
    }
    return 0;
}

int core_bl(void)
{
    if (registers[CMPR].value < 0) {
        return core_jmp();
    }
    return 0;
}

int core_ble (void)
{
    if (registers[CMPR].value <= 0) {
        return core_jmp();
    }
    return 0;
}

int core_bg  (void)
{
    if (registers[CMPR].value > 0) {
        return core_jmp();
    }
    return 0;
}

int core_bge (void)
{
    if (registers[CMPR].value >= 0) {
        return core_jmp();
    }
    return 0;
}

int core_jmp (void)
{
    /* to preserve the sign bit of the 3 bytes offset, 
     * we could check if the leftmost bit is one, and if it is, 
     * then OR the highest byte of the offset with 0xFF.
     * but it is easier if we just use ints for that:
     * converting from int8 to int32 will preserve the sign bit.
     */
    int8_t b8 = instruction[1];
    int32_t offset = b8;
    offset = (offset << 16) | (instruction[2] << 8) | instruction[3];


    /* the offset is the relative number of the instruction to jump to.
        2 means jump to the second instruction after this one (skip one),
       -1 means jump to the last instruction (offset -1)
       because an instruction is 4 bytes long, we multiply the
       offset with 4 */
    
    offset = offset * 4;
    registers[PC].value += offset;
    
    /* the machine increments the PC by 4 after every instruction,
       so we substract 4 in order to compensate the automatic increment */
    registers[PC].value -= 4;
    
    /* after this the machine does registers[PC].value += 4 
       and we hit the wanted offset */
    return 0;
}
