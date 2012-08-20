#include "core.h"      // instruction[]
#include "registers.h"

#include <stdio.h>

int core_add(void) 
{
    int32_t a = 0;
    int32_t b = 0;
    
    // try to read registers
    if (read_register(instruction[2], &a))     { return -1; }
    if (read_register(instruction[3], &b))     { return -1; }
    
    if (write_register(instruction[1], a + b)) { return -1; }
        
    return 0;
}

int core_addu(void)
{    
    uint32_t a = 0;
    uint32_t b = 0;
    
    // try to read registers
    if (read_registeru(instruction[2], &a))     { return -1; }
    if (read_registeru(instruction[3], &b))     { return -1; }
    
    if (write_registeru(instruction[1], a + b)) { return -1; }
        
    return 0;
}

int core_addi(void) 
{
    int32_t a = 0;
    if (read_register(instruction[2], &a))     { return -1; }
    int8_t b = instruction[3];
    
    if (write_register(instruction[1], a + b)) { return -1; }
        
    return 0;
}

int core_sub(void) 
{// copy paste from core_add
    int32_t a = 0;
    int32_t b = 0;
    
    // try to read registers
    if (read_register(instruction[2], &a))     { return -1; }
    if (read_register(instruction[3], &b))     { return -1; }
    
    // write result
    if (write_register(instruction[1], a - b)) { return -1; }
        
    return 0;
}

int core_subu(void) 
{// copy paste from addu, just the operation changed :)
    uint32_t a = 0;
    uint32_t b = 0;
    
    // try to read registers
    if (read_registeru(instruction[2], &a))     { return -1; }
    if (read_registeru(instruction[3], &b))     { return -1; }
    
    if (write_registeru(instruction[1], a - b)) { return -1; }
        
    return 0;
}

int core_subi(void)
{
    int32_t a = 0;
    if (read_register(instruction[2], &a))     { return -1; }
    int8_t b = instruction[3];
    
    if (write_register(instruction[1], a - b)) { return -1; }
        
    return 0;
}

int core_mul(void)
{
    int32_t a32 = 0;
    int32_t b32 = 0;
    
    // try to read registers
    if (read_register(instruction[1], &a32)) { return -1; }
    if (read_register(instruction[2], &b32)) { return -1; }
    
    // promomote to 64 bit arithmetic
    int64_t a = a32;
    int64_t b = b32;
    int64_t d = a * b;
    
    write_register(LO, (int32_t) d);
    write_register(HI, (int32_t) (d >> 32));
    
    return 0;
}

int core_mulu(void)
{
    // the machine works with 32 bit values
    uint32_t a32 = 0;
    uint32_t b32 = 0;
    
    // try to read registers
    if (read_registeru(instruction[1], &a32)) { return -1; }
    if (read_registeru(instruction[2], &b32)) { return -1; }
    
    // promomote to 64 bit arithmetic so we can get 64 bit results
    uint64_t a = a32;
    uint64_t b = b32;
    uint64_t d = a * b;
    
    write_registeru(LO, (uint32_t) d);
    write_registeru(HI, (uint32_t) (d >> 32));
    
    return 0;
}

int core_muli(void) 
{
    int32_t a32 = 0;
    int16_t b16 = 0; // the immediate factor is 16 bit
    
    // try to read the register
    if (read_register(instruction[1], &a32)) { return -1; }
    
    // extract the 16 bit factor from the instruction
    b16 = (instruction[2] << 8) | instruction[3];
    
    // promomote to 64 bit arithmetic for 64 bit results
    int64_t a = a32;
    int64_t b = b16;
    int64_t d = a * b;
    
    write_register(LO, (int32_t) d);
    write_register(HI, (int32_t) (d >> 32));
    return 0;
}

int core_div(void) 
{
    printf("%s\n", __func__);
    return 0;
}

int core_divu(void) 
{
    printf("%s\n", __func__);
    return 0;
}

int core_divi(void) 
{
    printf("%s\n", __func__);
    return 0;
}

int core_abs(void) 
{
    printf("%s\n", __func__);
    return 0;
}

int core_neg  (void)
{
    printf("%s\n", __func__);
    return 0;
}

int core_inc(void)
{
    int32_t r = 0;
    if (read_register(instruction[1], &r))     { return -1; }
    if (write_register(instruction[1], r + 1)) { return -1; }
    
    return 0;
}

int core_dec(void)
{
    int32_t r = 0;
    if (read_register(instruction[1], &r))     { return -1; }
    if (write_register(instruction[1], r - 1)) { return -1; }
    
    return 0;
}

int core_mod(void)
{
    printf("%s\n", __func__);
    return 0;
}

int core_modi (void)
{
    printf("%s\n", __func__);
    return 0;
}
