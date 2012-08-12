#include <stdint.h> // uint8_t
#include <string.h>

#include "command.h"
#include "disassemble.h"
#include "umach.h" // options

/*
 *
 * TODO: the functions print to stdout. Change that so that the user
 * has the control over printing and can use this functions in 
 * whatever scenario he likes.
 * 
 */

static
void disassemble(uint8_t inst[4])
{
    struct command *cmd = command_by_opcode(inst[0]);

    if (cmd == NULL) {
        printf("Unknown\n");
        return;
    }

    printf("%-5s ", cmd->opname);
    int n = 0;

    switch(cmd->format) {
        case NUL:
            break;
        case NNN:
            n = (inst[1] << 16) | (inst[2] << 8) | (inst[3]);

            if (n & 0x800000) {
                n = n | 0xFF000000;
            }

            printf("%d", n);
            break;
        case R00:
            printf("R%d", inst[1]);
            break;
        case RNN:
            printf("R%d ", inst[1]);
            n =  (inst[2] << 8) | (inst[3]);

            if (n & 0x8000) {
                n = n | 0xFF0000;
            }

            printf("%d", n);
            break;
        case RR0:
            printf("R%d R%d", inst[1], inst[2]);
            break;
        case RRN:
            printf("R%d R%d %d", inst[1], inst[2], inst[3]);
            break;
        case RRR:
            printf("R%d R%d R%d", inst[1], inst[2], inst[3]);
            break;
        default:
            printf("Unknown format");
    }

    printf("\n");
}

int disassemble_file(FILE* file)
{
    printf("Disassembly of file %s\n\n", options.filename);

    if (ferror(file)) {
        fprintf(stderr, "File error!\n");
        return -1;
    }

    if (feof(file)) {
        fprintf(stderr, "EOF detected\n");
        return -1;
    }

    uint8_t instruction[4];
    memset(instruction, 0, sizeof(instruction));

    while (fread(instruction, sizeof(*instruction), 4, file) > 0) {

        printf("%02X %02X %02X %02X\t",
               instruction[0],
               instruction[1],
               instruction[2],
               instruction[3]);
        disassemble(instruction);
        memset(instruction, 0, sizeof(instruction));
    }

    return 0;
}
