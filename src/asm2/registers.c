#include <glib.h>
#include <string.h>
#include "registers.h"

static const register_t umach_registers[] = {
    [REG_ZERO] = {REG_ZERO, "ZERO"},
    [REG_R1]   = {REG_R1,  "R1"},
    [REG_R2]   = {REG_R2,  "R2"},
    [REG_R3]   = {REG_R3,  "R3"},
    [REG_R4]   = {REG_R4,  "R4"},
    [REG_R5]   = {REG_R5,  "R5"},
    [REG_R6]   = {REG_R6,  "R6"},
    [REG_R7]   = {REG_R7,  "R7"},
    [REG_R8]   = {REG_R8,  "R8"},
    [REG_R9]   = {REG_R9,  "R9"},
    [REG_R10]  = {REG_R10, "R10"},
    [REG_R11]  = {REG_R11, "R11"},
    [REG_R12]  = {REG_R12, "R12"},
    [REG_R13]  = {REG_R13, "R13"},
    [REG_R14]  = {REG_R14, "R14"},
    [REG_R15]  = {REG_R15, "R15"},
    [REG_R16]  = {REG_R16, "R16"},
    [REG_R17]  = {REG_R17, "R17"},
    [REG_R18]  = {REG_R18, "R18"},
    [REG_R19]  = {REG_R19, "R19"},
    [REG_R20]  = {REG_R20, "R20"},
    [REG_R21]  = {REG_R21, "R21"},
    [REG_R22]  = {REG_R22, "R22"},
    [REG_R23]  = {REG_R23, "R23"},
    [REG_R24]  = {REG_R24, "R24"},
    [REG_R25]  = {REG_R25, "R25"},
    [REG_R26]  = {REG_R26, "R26"},
    [REG_R27]  = {REG_R27, "R27"},
    [REG_R28]  = {REG_R28, "R28"},
    [REG_R29]  = {REG_R29, "R29"},
    [REG_R30]  = {REG_R30, "R30"},
    [REG_R31]  = {REG_R31, "R31"},
    [REG_R32]  = {REG_R32, "R32"},   
    // special registers
    [REG_PC]   = {REG_PC,   "PC"},
    [REG_DS]   = {REG_DS,   "DS"},
    [REG_HS]   = {REG_HS,   "HS"},
    [REG_HE]   = {REG_HE,   "HE"},
    [REG_SP]   = {REG_SP,   "SP"},
    [REG_FP]   = {REG_FP,   "FP"},
    [REG_IR]   = {REG_IR,   "IR"},
    [REG_STAT] = {REG_STAT, "STAT"},
    [REG_ERR]  = {REG_ERR,  "ERR"},
    [REG_HI]   = {REG_HI,   "HI"},
    [REG_LO]   = {REG_LO,   "LO"},
    [REG_CMPR] = {REG_CMPR, "CMPR"}
};

static GHashTable *registers_ht = NULL;

static void init_registers_ht() {
	registers_ht = g_hash_table_new(g_str_hash, g_str_equal);
	
	const int reg_cnt = sizeof(umach_registers) / sizeof(register_t);
	
	for (int i = 0; i < reg_cnt; i++)
		g_hash_table_insert(registers_ht, umach_registers[i].regname,
		                    (gpointer) &(umach_registers[i]));
}

int get_register_by_name(const char *name, register_t *result) {
	if (registers_ht == NULL)
		init_registers_ht();
		
    char *reg_name = g_ascii_strup(name, -1);
    register_t *reg = g_hash_table_lookup(registers_ht, reg_name);
    g_free(reg_name);
	
	if (reg != NULL) {
		result->regname = reg->regname;
		result->regcode = reg->regcode;
		return TRUE;
	} else
		return FALSE;
}

