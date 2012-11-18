#ifndef COMMANDS_H
#define COMMANDS_H

#include <stdint.h>

typedef enum {
    CMDFMT_NUL, CMDFMT_NNN, CMDFMT_R00, CMDFMT_RNN,
    CMDFMT_RR0, CMDFMT_RRN, CMDFMT_RRR
} cmdformat_t;

typedef struct {
    uint8_t     opcode;
    char       *opname;
    cmdformat_t format;
    char        has_label;
} command_t;

int  get_command_by_name(const char *name, command_t *result);
void free_commands_ht();
const char *get_cmdfmt_name(cmdformat_t fmt);

#endif
