#include <stdint.h> // uint8_t
#include <string.h>

#include "command.h"
#include "disassemble.h"
#include "umach.h" // options

void disassemble(const uint8_t instruction[4], char *destination, int printhex)
{
    const uint8_t *ins = instruction; // shorter name
    char buffer[128] = {'\0'}; // char buffer we use to sprintf stuff
    
    if (printhex) {
        sprintf(destination,
            "%02X %02X %02X %02X\t", ins[0], ins[1], ins[2], ins[3]);
    }    

    struct command *cmd = command_by_opcode(ins[0]);
    
    if (cmd == NULL) {
        sprintf(buffer, "Unknown command: 0x%02X", ins[0]);
        strcat(destination, buffer);
        return;
    }
    
    sprintf(buffer, "%-5s ", cmd->opname);
    strcat(destination, buffer);

    int n = 0;

    switch(cmd->format) {
        case NUL:
            break;
        case NNN:
            n = (ins[1] << 16) | (ins[2] << 8) | (ins[3]);

            if (n & 0x800000) {// extend the sign bit
                n = n | 0xFF000000;
            }

            sprintf(buffer, "%d", n);
            break;
        case R00:
            sprintf(buffer, "R%d", ins[1]);
            break;
        case RNN:
            sprintf(buffer, "R%d ", ins[1]);
            n =  (ins[2] << 8) | (ins[3]);

            if (n & 0x8000) {// sign bit
                n = n | 0xFF0000;
            }

            sprintf(buffer, "%d", n);
            break;
        case RR0:
            sprintf(buffer, "R%d R%d", ins[1], ins[2]);
            break;
        case RRN:
            sprintf(buffer, "R%d R%d %d", ins[1], ins[2], ins[3]);
            break;
        case RRR:
            sprintf(buffer, "R%d R%d R%d", ins[1], ins[2], ins[3]);
            break;
        default:
            sprintf(buffer, "Unknown format");
    }

    strcat(destination, buffer);
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

    uint8_t instruction[4] = {0};
    char line[128] = {'\0'};

    while (fread(instruction, sizeof(*instruction), 4, file) > 0) {
        disassemble(instruction, line, 1);
        printf("%s\n", line);
        // clear buffers
        memset(instruction, 0, sizeof(instruction));
        memset(line, 0, sizeof(line));
    }

    return 0;
}
