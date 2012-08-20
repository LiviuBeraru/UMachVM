#include "core.h"
#include "memory.h"
#include "registers.h"

#include <stdio.h>

int core_set(void)
{
    // extract the 16 bit argument
    int16_t n = (instruction[2] << 8) | instruction[3];

    if (write_register(instruction[1], n)) {
        return -1;
    } else {
        return 0;
    }
}

int core_cp(void)
{
    int32_t value = 0;
    if (read_register(instruction[2], &value)) {
        return -1;
    }
    if (write_register(instruction[1], value)) {
        return -1;
    }

    return 0;
}

int core_lb(void)
{
    int32_t address = 0;
    int32_t value = 0;
    uint8_t mbyte = 0;

    // read the memory address from register
    if (read_register(instruction[2], &address)) {
        return -1;
    }

    // read the byte from memory
    if (mem_read(&mbyte, address, 1)) {
        return -1;
    }

    // read the old value
    if (read_register(instruction[1], &value)) {
        return -1;
    }

    // set last byte of the new value
    value = value & 0xFFFFFF00; // unset last byte
    value = value | mbyte;      // set last byte

    // write new value
    if (write_register(instruction[1], value)) {
        return -1;
    }
    return 0;
}

int core_lw(void)
{
    printf("%s\n", __func__);
    return 0;
}

int core_sb(void)
{
    int32_t address = 0;
    int32_t rvalue = 0;
    uint8_t  byte = 0;
    
    // read the value to store from register
    if (read_register(instruction[1], &rvalue)) {
        return -1;
    }

    // read the memory address from register
    if (read_register(instruction[2], &address)) {
        return -1;
    }

    // take the last byte
    byte = (uint8_t) rvalue;

    if (mem_write(&byte, address, 1)) {
        return -1;
    }
    return 0;
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

