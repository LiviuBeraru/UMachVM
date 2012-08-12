#include "command.h"
#include "controll.h"
#include "loadstore.h"
#include "arithm.h"
#include "logic.h"
#include "compare.h"
#include "branch.h"
#include "subroutine.h"
#include "system.h"
#include "io.h"

#include <stddef.h>

static
struct command opmap[OPMAX] = {
    [0x00] = {0x00,  "NOP"  ,  core_nop  , NUL},
    [0x04] = {0x04,  "EOP"  ,  core_eop  , NUL},
    [0x10] = {0x10,  "SET"  ,  core_set  , RNN},
    [0x11] = {0x11,  "CP"   ,  core_cp   , RR0},
    [0x12] = {0x12,  "LB"   ,  core_lb   , RR0},
    [0x13] = {0x13,  "LW"   ,  core_lw   , RR0},
    [0x14] = {0x14,  "SB"   ,  core_sb   , RR0},
    [0x15] = {0x15,  "SW"   ,  core_sw   , RR0},
    [0x18] = {0x18,  "PUSH" ,  core_push , R00},
    [0x19] = {0x19,  "POP"  ,  core_pop  , R00},
    [0x30] = {0x30,  "ADD"  ,  core_add  , RRR},
    [0x31] = {0x31,  "ADDU" ,  core_addu , RRR},
    [0x32] = {0x32,  "ADDI" ,  core_addi , RRN},
    [0x33] = {0x33,  "SUB"  ,  core_sub  , RRR},
    [0x34] = {0x34,  "SUBU" ,  core_subu , RRN},
    [0x35] = {0x35,  "SUBI" ,  core_subi , RRN},
    [0x38] = {0x38,  "MUL"  ,  core_mul  , RR0},
    [0x39] = {0x39,  "MULU" ,  core_mulu , RR0},
    [0x3A] = {0x3A,  "MULI" ,  core_muli , RNN},
    [0x3B] = {0x3B,  "DIV"  ,  core_div  , RR0},
    [0x3C] = {0x3C,  "DIVU" ,  core_divu , RR0},
    [0x3D] = {0x3D,  "DIVI" ,  core_divi , RNN},            
    [0x40] = {0x40,  "ABS"  ,  core_abs  , RR0},
    [0x41] = {0x41,  "NEG"  ,  core_neg  , RR0},
    [0x42] = {0x42,  "INC"  ,  core_inc  , R00},
    [0x43] = {0x43,  "DEC"  ,  core_dec  , R00},
    [0x48] = {0x48,  "MOD"  ,  core_mod  , RRR},
    [0x49] = {0x49,  "MODI" ,  core_modi , RRN},
    [0x50] = {0x50,  "AND"  ,  core_and  , RRR},
    [0x51] = {0x51,  "ANDI" ,  core_andi , RRN},
    [0x52] = {0x52,  "OR"   ,  core_or   , RRR},
    [0x53] = {0x53,  "ORI"  ,  core_ori  , RRN},
    [0x54] = {0x54,  "XOR"  ,  core_xor  , RRR},
    [0x55] = {0x55,  "XORI" ,  core_xori , RRN},
    [0x56] = {0x56,  "NOT"  ,  core_not  , RR0},
    [0x57] = {0x57,  "NOTI" ,  core_noti , RNN},
    [0x58] = {0x58,  "NAND" ,  core_nand , RRR},
    [0x59] = {0x59,  "NANDI",  core_nandi, RRN},
    [0x5A] = {0x5A,  "NOR"  ,  core_nor  , RRR},
    [0x5B] = {0x5B,  "NORI" ,  core_nori , RRN},
    [0x60] = {0x60,  "SHL"  ,  core_shl  , RRR},
    [0x61] = {0x61,  "SHLI" ,  core_shli , RRN},
    [0x62] = {0x62,  "SHR"  ,  core_shr  , RRR},
    [0x63] = {0x63,  "SHRI" ,  core_shri , RRN},
    [0x64] = {0x64,  "SHRA" ,  core_shra , RRR},
    [0x65] = {0x65,  "SHRAI",  core_shrai, RRN},
    [0x68] = {0x68,  "ROTL" ,  core_rotl , RRR},
    [0x69] = {0x69,  "ROTLI",  core_rotli, RRN},
    [0x6A] = {0x6A,  "ROTR" ,  core_rotr , RRR},
    [0x6B] = {0x6B,  "ROTRI",  core_rotri, RRN},
    [0x70] = {0x70,  "CMP"  ,  core_cmp  , RR0},
    [0x71] = {0x71,  "CMPU" ,  core_cmpu , RR0},
    [0x72] = {0x72,  "CMPI" ,  core_cmpi , RRN},
    [0x80] = {0x80,  "BE"   ,  core_be   , NNN},
    [0x81] = {0x81,  "BNE"  ,  core_bne  , NNN},
    [0x82] = {0x82,  "BL"   ,  core_bl   , NNN},
    [0x83] = {0x83,  "BLE"  ,  core_ble  , NNN},
    [0x84] = {0x84,  "BG"   ,  core_bg   , NNN},
    [0x85] = {0x85,  "BGE"  ,  core_bge  , NNN},
    [0x88] = {0x88,  "JMP"  ,  core_jmp  , NNN},
    [0x90] = {0x90,  "GO"   ,  core_go   , R00},
    [0x91] = {0x91,  "CALL" ,  core_call , NNN},
    [0x92] = {0x92,  "RET"  ,  core_ret  , NUL},
    [0xA0] = {0xA0,  "INT"  ,  core_int  , NNN},
    [0xB0] = {0xB0,  "IN"   ,  core_in   , RRR},
    [0xB8] = {0xB8,  "OUT"  ,  core_out  , RRR}
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
