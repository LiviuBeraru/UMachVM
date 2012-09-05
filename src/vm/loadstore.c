#include "core.h"
#include "memory.h"
#include "registers.h"
#include "logmsg.h"
#include "system.h"     // interrupt()
#include "interrupts.h" // interupt numbers

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
    int dest_reg = instruction[1];
    int adr_reg  = instruction[2];
    
    int32_t address = 0;
    int32_t mword   = 0;

    // read the address from register
    if (read_register(adr_reg, &address) == -1) {
        return -1;
    }
    
    uint8_t buffer[4] = { 0x0 };

    // read memory
    if (mem_read(buffer, address, 4) == -1) {
        return -1;
    }
    
    /* on little endian machines, like intel, 
     * the byte order of an integer in memory is reversed,
     * so when we read 4 bytes from memory as a 4-byte-integer, 
     * we have to mirror the bytes in order to get the actual value
     */
    
    mword |= (buffer[0] << 24);
    mword |= (buffer[1] << 16);
    mword |= (buffer[2] <<  8);
    mword |= (buffer[3] <<  0);
    
    // write memory word back into the destination register
    if (write_register(dest_reg, mword) == -1) {
        return -1;
    }
    
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
    uint8_t reg_no = instruction[1];
    uint8_t adr_no = instruction[2];
    
    int32_t reg_value = 0;
    int32_t address = 0;
    
    if (read_register(reg_no, &reg_value) == -1) { return -1; }
    if (read_register(adr_no, &address) == -1)   { return -1; }
    
    /* if we just cast the register value reg_value to an array of uint, 
     * the 4 array elements (the bytes of the int) will have the reverse order
     * they should have. Reason is that we use a little endian machine (Intel).
     * So we deal we this by storing the bytes of the integer into a buffer 
     * in the right order (big endian).
     */
    uint8_t buffer[4] = { 0x0 };
    buffer[0] = reg_value >> 24;
    buffer[1] = reg_value >> 16;
    buffer[2] = reg_value >>  8;
    buffer[3] = reg_value;
    
    /* the memory module interrupts if address is not ok */
    if (mem_write(buffer, address, 4) == -1) { return -1; }
    
    return 0;
}

int core_push(void)
{
    uint8_t reg_no = instruction[1];
    int32_t reg_value = 0;
    if (read_register(reg_no, &reg_value) == -1) { return -1; }
    
    registers[SP].value -= 4;
    if (registers[SP].value < 0) {
        /* We generate a stack overflow interrupt if the 
         * SP register has a value less than zero. Future versions
         * should generate an interrupt if the SP register points
         * to a memory region which is occupied by the programm itself.
         * To do this one should store the program size in the memory.c module.
         */
        logmsg(LOG_WARN, "Stack Overflow: cannot PUSH");
        registers[SP].value += 4; // reset SP
        interrupt(INT_STACK_OVERFLOW);
        return -1;
    }
    
    uint8_t buffer[4] = { 0x0 };
    buffer[0] = reg_value >> 24;
    buffer[1] = reg_value >> 16;
    buffer[2] = reg_value >>  8;
    buffer[3] = reg_value;
    
    if (mem_write(buffer, registers[SP].value, 4) == -1) { return -1; }
    
    return 0;
}

int core_pop(void)
{
    // register number
    uint8_t reg_no = instruction[1];
    // register value, which we read from memory
    int32_t reg_value = 0;
    
    uint8_t buffer[4] = { 0x0 };
    
    if (mem_read(buffer, registers[SP].value, 4) == -1) {
        logmsg(LOG_WARN, "Stack Error: cannot POP");
        interrupt(INT_STACK_ERR);
        return -1;
    }
    
    /* disassemble the buffer into the register value */
    reg_value = (buffer[0] << 24) | 
                (buffer[1] << 16) | 
                (buffer[2] <<  8) | 
                 buffer[3];
    
    /* write the value into the actual register */
    
    if (write_register(reg_no, reg_value) == -1) { return -1; }
    registers[SP].value += 4;
    
    return 0;
}

