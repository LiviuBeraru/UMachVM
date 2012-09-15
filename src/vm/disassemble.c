#include <stdio.h>
#include <string.h>
#include "disassemble.h"
#include "command.h"
#include "umach.h" // options
#include "registers.h"
#include "memory.h" // begin_data

/* These functions implement the different instruction formats */
static void formatNNN(const uint8_t ins[4], char *dest);
static void formatR00(const uint8_t ins[4], char *dest);
static void formatRNN(const uint8_t ins[4], char *dest);
static void formatRR0(const uint8_t ins[4], char *dest);
static void formatRRN(const uint8_t ins[4], char *dest);
static void formatRRR(const uint8_t ins[4], char *dest);
static int  disassemble_file(FILE* file);

void disassemble(const uint8_t instruction[4], char *destination, int printhex)
{
    if (printhex) {
        sprintf(destination,
                "%02X %02X %02X %02X\t",
                instruction[0], instruction[1],
                instruction[2], instruction[3]);
    }

    char buffer[64];
    struct command *cmd = command_by_opcode(instruction[0]);
    if (cmd == NULL) {
        strcat(destination, "???");
        return;
    }


    sprintf(buffer, "%-5s ", cmd->opname);
    strcat(destination, buffer);

    switch(cmd->format) {
        case NUL:
            break;
        case NNN:
            formatNNN(instruction, destination);
            break;
        case R00:
            formatR00(instruction, destination);
            break;
        case RNN:
            formatRNN(instruction, destination);
            break;
        case RR0:
            formatRR0(instruction, destination);
            break;
        case RRN:
            formatRRN(instruction, destination);
            break;
        case RRR:
            formatRRR(instruction, destination);
            break;
        default:
            sprintf(buffer, "Unknown format");
    }
}

int disassemble_files(int argc, char** argv)
{
    int i = 0;
    FILE *file;
    for ( ; i < argc; i++) {
        file = fopen(argv[i], "r");
        if (file == NULL) {
            perror(argv[0]);
            break;
        }

        printf("\nDisassembly of file %s\n\n", argv[i]);
        disassemble_file(file);
        fclose(file);
    }
    return i;
}


int disassemble_file(FILE* file)
{
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

        if (memcmp(instruction, begin_data, 4) == 0) {
            /* we have found the data mark, stop disassembling because
               after the data mark there are no instructions any more */
            break;
        }
        
        disassemble(instruction, line, 1);
        printf("%s\n", line);
        // clear buffers
        memset(instruction, 0, sizeof(instruction));
        memset(line, 0, sizeof(line));

    }

    return 0;
}

void formatNNN(const uint8_t ins[4], char* dest)
{
    int8_t n8 = ins[1];
    int32_t n = n8;
    n = (n << 16) | (ins[2] << 8) | (ins[3]);

    /* we could have set the sign bit also this way:
    if (n & 0x800000) {// leftmost bit is set
        n = n | 0xFF000000;
    }
    but converting int8 to int32 will automatically
    preserve the sign bit
    */
    char buffer[16];
    if (options.hexa) {
        sprintf(buffer, "0x%X", n);
    } else {
        sprintf(buffer, "%d", n);
    }
    strcat(dest, buffer);
}

void formatR00(const uint8_t ins[4], char* dest)
{
    Register *r = get_register(ins[1]);
    if (r) {
        strcat(dest, r->name);
    } else {
        strcat(dest, "???");
    }
}

void formatRNN(const uint8_t ins[4], char* dest)
{
    Register *r = get_register(ins[1]);
    char buffer[16];

    if (r) {
        sprintf(buffer, "%-5s ", r->name);
    } else {
        sprintf(buffer, "%-5s ", "???");
    }
    strcat(dest, buffer);


    int16_t n =  (ins[2] << 8) | (ins[3]);
    if (options.hexa) {
        sprintf(buffer, "0x%X", n);
    } else {
        sprintf(buffer, "%d", n);
    }
    strcat(dest, buffer);
}

void formatRR0(const uint8_t ins[4], char* dest)
{
    Register *a = get_register(ins[1]);
    Register *b = get_register(ins[2]);

    char buffer[8];

    if (a) {
        sprintf(buffer, "%-5s ", a->name);
    } else {
        sprintf(buffer, "%-5s ", "???");
    }
    strcat(dest, buffer);

    if (b) {
        sprintf(buffer, "%-5s ", b->name);
    } else {
        sprintf(buffer, "%-5s ", "???");
    }
    strcat(dest, buffer);
}

void formatRRN(const uint8_t ins[4], char* dest)
{
    formatRR0(ins, dest);
    char buffer[16];
    int8_t n = ins[3];
    if (options.hexa) {
        sprintf(buffer, "0x%X", n);
    } else {
        sprintf(buffer, "%d", n);
    }
    strcat(dest, buffer);
}

void formatRRR(const uint8_t ins[4], char* dest)
{
    formatRR0(ins, dest);
    Register *a = get_register(ins[3]);
    if (a) {
        strcat(dest, a->name);
    } else {
        strcat(dest, "???");
    }
}
