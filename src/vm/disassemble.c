#include <string.h>
#include "disassemble.h"
#include "command.h"
#include "umach.h" // options
#include "registers.h"

/* These functions implement the different instruction formats */
static void formatNNN(const uint8_t ins[4], char *dest);
static void formatR00(const uint8_t ins[4], char *dest);
static void formatRNN(const uint8_t ins[4], char *dest);
static void formatRR0(const uint8_t ins[4], char *dest);
static void formatRRN(const uint8_t ins[4], char *dest);
static void formatRRR(const uint8_t ins[4], char *dest);

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
        
        if (instruction[0] == 0x04) {
            /* don't disassemble beyond the EOP instruction,
             * which has the opcode 0x04 */
            break;
        } else {
            // clear buffers
            memset(instruction, 0, sizeof(instruction));
            memset(line, 0, sizeof(line));
        }
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
    sprintf(buffer, "%d", n);
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
    sprintf(buffer, "%d", n);
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
    sprintf(buffer, "%d", ins[3]);
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
