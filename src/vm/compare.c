#include "core.h"
#include "registers.h"

int core_cmp  (void)
{
    int32_t a = 0;
    int32_t b = 0;
    
    if (read_register(instruction[1], &a)) { return -1; }
    if (read_register(instruction[2], &b)) { return -1; }

    if (a == b) {
        registers[CMPR].value =  0;
    } else if (a < b) {
        registers[CMPR].value = -1;
    } else {
        registers[CMPR].value =  1;
    }
    
    return 0;
}

int core_cmpu (void)
{
    int rega_no = instruction[1];
    int regb_no = instruction[2];
    
    uint32_t rega_value = 0;
    uint32_t regb_value = 0;
    
    if (read_registeru(rega_no, & rega_value) == -1) { return -1; }
    if (read_registeru(regb_no, & regb_value) == -1) { return -1; }
    
    if (rega_value == regb_value) {
        registers[CMPR].value =  0;
    } else if (rega_value < regb_value) {
        registers[CMPR].value = -1;
    } else {
        registers[CMPR].value =  1;
    }
    
    return 0;
}

int core_cmpi (void)
{
    int regno = instruction[1];
    int16_t immediate = (instruction[2] << 8) | instruction[3];
    
    int32_t value = 0;
    if (read_register(regno, & value) == -1) { return -1; }
    
    if (value == immediate) {
        registers[CMPR].value =  0;
    } else if (value < immediate) {
        registers[CMPR].value = -1;
    } else {
        registers[CMPR].value =  1;
    }
    
    return 0;
}
