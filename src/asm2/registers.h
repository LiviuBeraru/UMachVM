#ifndef REGISTERS_H
#define REGISTERS_H

#include <stdint.h>

typedef struct {
    uint8_t regcode;
    char   *regname;
} register_t;

typedef enum {
    REG_ZERO = 0,    // zero register
    REG_R1,  REG_R2,  REG_R3,  REG_R4,  REG_R5,  REG_R6,  REG_R7,  REG_R8,
    REG_R9,  REG_R10, REG_R11, REG_R12, REG_R13, REG_R14, REG_R15, REG_R16,
    REG_R17, REG_R18, REG_R19, REG_R20, REG_R21, REG_R22, REG_R23, REG_R24,  
    REG_R25, REG_R26, REG_R27, REG_R28, REG_R29, REG_R30, REG_R31, REG_R32,
    REG_PC   = 33,   // program counter
    REG_DS   = 34,   // data segment begin
    REG_HS   = 35,   // heap segment begin
    REG_HE   = 36,   // heap segment end (last byte)
    REG_SP   = 37,   // stack pointer
    REG_FP   = 38,   // frame pointer
    REG_IR   = 39,   // instruction register
    REG_STAT = 40,   // status register
    REG_ERR  = 41,   // error register
    REG_HI   = 42,   // higher 32 bits of mul/div
    REG_LO   = 43,   // lower 32 bits of mul/div
    REG_CMPR = 44    // comparison result register
} register_codes_t;

int get_register_by_name(const char *name, register_t *result);

#endif
