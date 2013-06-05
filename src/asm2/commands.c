/*
 * commands.c
 * 
 * translate command names to command_t
 * see get_command_by_name()
 * 
 */

#include "commands.h"

#include <glib.h>
#include <stddef.h> // NULL
#include <string.h> // strcasecmp

static const command_t umach_commands[] = {
    {0x00, "NOP",   CMDFMT_NUL, FALSE},
    {0x04, "EOP",   CMDFMT_NUL, FALSE},
    {0x10, "SET",   CMDFMT_RNN, FALSE},
    {0x11, "CP",    CMDFMT_RR0, FALSE},
    {0x12, "LB",    CMDFMT_RR0, FALSE},
    {0x13, "LW",    CMDFMT_RR0, FALSE},
    {0x14, "SB",    CMDFMT_RR0, FALSE},
    {0x15, "SW",    CMDFMT_RR0, FALSE},
    {0x18, "PUSH",  CMDFMT_R00, FALSE},
    {0x19, "POP",   CMDFMT_R00, FALSE},
    {0x30, "ADD",   CMDFMT_RRR, FALSE},
    {0x31, "ADDU",  CMDFMT_RRR, FALSE},
    {0x32, "ADDI",  CMDFMT_RRN, FALSE},
    {0x33, "SUB",   CMDFMT_RRR, FALSE},
    {0x34, "SUBU",  CMDFMT_RRR, FALSE},
    {0x35, "SUBI",  CMDFMT_RRN, FALSE},
    {0x36, "SUBI2", CMDFMT_RRN, FALSE},
    {0x38, "MUL",   CMDFMT_RR0, FALSE},
    {0x39, "MULU",  CMDFMT_RR0, FALSE},
    {0x3A, "MULI",  CMDFMT_RNN, FALSE},
    {0x3B, "DIV",   CMDFMT_RR0, FALSE},
    {0x3C, "DIVU",  CMDFMT_RR0, FALSE},
    {0x3D, "DIVI",  CMDFMT_RNN, FALSE},
    {0x3E, "DIVI2", CMDFMT_RNN, FALSE},
    {0x40, "ABS",   CMDFMT_RR0, FALSE},
    {0x41, "NEG",   CMDFMT_RR0, FALSE},
    {0x42, "INC",   CMDFMT_R00, FALSE},
    {0x43, "DEC",   CMDFMT_R00, FALSE},
    {0x48, "MOD",   CMDFMT_RRR, FALSE},
    {0x49, "MODI",  CMDFMT_RRN, FALSE},
    {0x4A, "MODI2", CMDFMT_RRN, FALSE},
    {0x50, "AND",   CMDFMT_RRR, FALSE},
    {0x51, "ANDI",  CMDFMT_RRN, FALSE},
    {0x52, "OR",    CMDFMT_RRR, FALSE},
    {0x53, "ORI",   CMDFMT_RRN, FALSE},
    {0x54, "XOR",   CMDFMT_RRR, FALSE},
    {0x55, "XORI",  CMDFMT_RRN, FALSE},
    {0x56, "NOT",   CMDFMT_RR0, FALSE},
    {0x57, "NOTI",  CMDFMT_RNN, FALSE},
    {0x58, "NAND",  CMDFMT_RRR, FALSE},
    {0x59, "NANDI", CMDFMT_RRN, FALSE},
    {0x5A, "NOR",   CMDFMT_RRR, FALSE},
    {0x5B, "NORI",  CMDFMT_RRN, FALSE},
    {0x60, "SHL",   CMDFMT_RRR, FALSE},
    {0x61, "SHLI",  CMDFMT_RRN, FALSE},
    {0x62, "SHR",   CMDFMT_RRR, FALSE},
    {0x63, "SHRI",  CMDFMT_RRN, FALSE},
    {0x64, "SHRA",  CMDFMT_RRR, FALSE},
    {0x65, "SHRAI", CMDFMT_RRN, FALSE},
    {0x68, "ROTL",  CMDFMT_RRR, FALSE},
    {0x69, "ROTLI", CMDFMT_RRN, FALSE},
    {0x6A, "ROTR",  CMDFMT_RRR, FALSE},
    {0x6B, "ROTRI", CMDFMT_RRN, FALSE},
    {0x70, "CMP",   CMDFMT_RR0, FALSE},
    {0x71, "CMPU",  CMDFMT_RR0, FALSE},
    {0x72, "CMPI",  CMDFMT_RNN, FALSE},
    {0x80, "BE",    CMDFMT_NNN, TRUE},
    {0x81, "BNE",   CMDFMT_NNN, TRUE},
    {0x82, "BL",    CMDFMT_NNN, TRUE},
    {0x83, "BLE",   CMDFMT_NNN, TRUE},
    {0x84, "BG",    CMDFMT_NNN, TRUE},
    {0x85, "BGE",   CMDFMT_NNN, TRUE},
    {0x88, "JMP",   CMDFMT_NNN, TRUE},
    {0x90, "GO",    CMDFMT_R00, FALSE},
    {0x91, "CALL",  CMDFMT_NNN, TRUE},
    {0x92, "RET",   CMDFMT_NUL, FALSE},
    {0xA0, "INT",   CMDFMT_NNN, FALSE},
    {0xB0, "IN",    CMDFMT_RRR, FALSE},
    {0xB8, "OUT",   CMDFMT_RRR, FALSE}
};

static const char *umach_cmdfmt_names[] = {
    [CMDFMT_NUL] = "NUL: nothing",
    [CMDFMT_NNN] = "NNN: NUM(24) or LABEL",
    [CMDFMT_R00] = "R00: REG",
    [CMDFMT_RNN] = "RNN: REG, NUM(16)",
    [CMDFMT_RR0] = "RR0: REG, REG",
    [CMDFMT_RRN] = "RRN: REG, REG, NUM(8)",
    [CMDFMT_RRR] = "RRR: REG, REG, REG"
};

static GHashTable *commands_ht = NULL;

static void init_commands_ht() {
    commands_ht = g_hash_table_new(g_str_hash, g_str_equal);

    const int cmd_cnt = sizeof(umach_commands) / sizeof(command_t);

    for (int i = 0; i < cmd_cnt; i++)
        g_hash_table_insert(commands_ht, umach_commands[i].opname,
                            (gpointer) &(umach_commands[i]));
}

int get_command_by_name(const char *name, command_t *result)
{
    if (commands_ht == NULL)
        init_commands_ht();

    char *cmd_name = g_ascii_strup(name, -1);
    command_t *cmd = g_hash_table_lookup(commands_ht, cmd_name);
    g_free(cmd_name);

    if (cmd != NULL) {
        *result = *cmd;
        return TRUE;
    } else
        return FALSE;
}

void free_commands_ht() {
    if (commands_ht == NULL)
        return;

    g_hash_table_destroy(commands_ht);
    commands_ht = NULL;
}

const char *get_cmdfmt_name(cmdformat_t fmt) {
    return umach_cmdfmt_names[fmt];
}
