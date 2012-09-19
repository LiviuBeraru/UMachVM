#include "command.h"

#include <stddef.h> // NULL
#include <string.h> // strcasecmp

static
struct command opmap[OPMAX] = {
    [0x00] = {0x00,  "NOP"  ,  NUL,  0},
    [0x04] = {0x04,  "EOP"  ,  NUL,  0},
    [0x10] = {0x10,  "SET"  ,  RNN,  0},
    [0x11] = {0x11,  "CP"   ,  RR0,  0},
    [0x12] = {0x12,  "LB"   ,  RR0,  0},
    [0x13] = {0x13,  "LW"   ,  RR0,  0},
    [0x14] = {0x14,  "SB"   ,  RR0,  0},
    [0x15] = {0x15,  "SW"   ,  RR0,  0},
    [0x18] = {0x18,  "PUSH" ,  R00,  0},
    [0x19] = {0x19,  "POP"  ,  R00,  0},
    [0x30] = {0x30,  "ADD"  ,  RRR,  0},
    [0x31] = {0x31,  "ADDU" ,  RRR,  0},
    [0x32] = {0x32,  "ADDI" ,  RRN,  0},
    [0x33] = {0x33,  "SUB"  ,  RRR,  0},
    [0x34] = {0x34,  "SUBU" ,  RRR,  0},
    [0x35] = {0x35,  "SUBI" ,  RRN,  0},
    [0x36] = {0x36,  "SUBI2",  RRN,  0},
    [0x38] = {0x38,  "MUL"  ,  RR0,  0},
    [0x39] = {0x39,  "MULU" ,  RR0,  0},
    [0x3A] = {0x3A,  "MULI" ,  RNN,  0},
    [0x3B] = {0x3B,  "DIV"  ,  RR0,  0},
    [0x3C] = {0x3C,  "DIVU" ,  RR0,  0},
    [0x3D] = {0x3D,  "DIVI" ,  RNN,  0},
    [0x3E] = {0x3E,  "DIVI2",  RNN,  0},
    [0x40] = {0x40,  "ABS"  ,  RR0,  0},
    [0x41] = {0x41,  "NEG"  ,  RR0,  0},
    [0x42] = {0x42,  "INC"  ,  R00,  0},
    [0x43] = {0x43,  "DEC"  ,  R00,  0},
    [0x48] = {0x48,  "MOD"  ,  RRR,  0},
    [0x49] = {0x49,  "MODI" ,  RRN,  0},
    [0x4A] = {0x4A,  "MODI2",  RRN,  0},
    [0x50] = {0x50,  "AND"  ,  RRR,  0},
    [0x51] = {0x51,  "ANDI" ,  RRN,  0},
    [0x52] = {0x52,  "OR"   ,  RRR,  0},
    [0x53] = {0x53,  "ORI"  ,  RRN,  0},
    [0x54] = {0x54,  "XOR"  ,  RRR,  0},
    [0x55] = {0x55,  "XORI" ,  RRN,  0},
    [0x56] = {0x56,  "NOT"  ,  RR0,  0},
    [0x57] = {0x57,  "NOTI" ,  RNN,  0},
    [0x58] = {0x58,  "NAND" ,  RRR,  0},
    [0x59] = {0x59,  "NANDI",  RRN,  0},
    [0x5A] = {0x5A,  "NOR"  ,  RRR,  0},
    [0x5B] = {0x5B,  "NORI" ,  RRN,  0},
    [0x60] = {0x60,  "SHL"  ,  RRR,  0},
    [0x61] = {0x61,  "SHLI" ,  RRN,  0},
    [0x62] = {0x62,  "SHR"  ,  RRR,  0},
    [0x63] = {0x63,  "SHRI" ,  RRN,  0},
    [0x64] = {0x64,  "SHRA" ,  RRR,  0},
    [0x65] = {0x65,  "SHRAI",  RRN,  0},
    [0x68] = {0x68,  "ROTL" ,  RRR,  0},
    [0x69] = {0x69,  "ROTLI",  RRN,  0},
    [0x6A] = {0x6A,  "ROTR" ,  RRR,  0},
    [0x6B] = {0x6B,  "ROTRI",  RRN,  0},
    [0x70] = {0x70,  "CMP"  ,  RR0,  0},
    [0x71] = {0x71,  "CMPU" ,  RR0,  0},
    [0x72] = {0x72,  "CMPI" ,  RNN,  0},
    [0x80] = {0x80,  "BE"   ,  NNN,  1},
    [0x81] = {0x81,  "BNE"  ,  NNN,  1},
    [0x82] = {0x82,  "BL"   ,  NNN,  1},
    [0x83] = {0x83,  "BLE"  ,  NNN,  1},
    [0x84] = {0x84,  "BG"   ,  NNN,  1},
    [0x85] = {0x85,  "BGE"  ,  NNN,  1},
    [0x88] = {0x88,  "JMP"  ,  NNN,  1},
    [0x90] = {0x90,  "GO"   ,  R00,  0},
    [0x91] = {0x91,  "CALL" ,  NNN,  1},
    [0x92] = {0x92,  "RET"  ,  NUL,  0},
    [0xA0] = {0xA0,  "INT"  ,  NNN,  0},
    [0xB0] = {0xB0,  "IN"   ,  RRR,  0},
    [0xB8] = {0xB8,  "OUT"  ,  RRR,  0}
};

struct command* command_by_opcode(int opcode)
{
    if (opcode < 0 || opcode >= OPMAX) {
        return NULL;
    }
    
    if (opmap[opcode].opname) {
        return & opmap[opcode];
    } else {
        return NULL;
    }
}

struct command* command_by_name(const char *name)
{
    struct command *cmd = opmap;
    int i = 0;
    while (i < OPMAX) {
        if (cmd->opname && (strcasecmp(cmd->opname, name) == 0)) {
            return cmd;
        }
        
        cmd++;
        i++;
    }
    
    return NULL;
}
