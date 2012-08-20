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


    /* the offset is the number of instructions to jump over.
       because an instruction is 4 bytes long, we multiply the
       offset with 4 */
    offset = offset * 4;
    registers[PC].value += offset;

    if (offset < 0) {
        /* jumping back needs an adjustment */
        registers[PC].value -= 8;
    }
    return 0;
}
