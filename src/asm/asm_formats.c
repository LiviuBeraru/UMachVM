#include <stddef.h>     // size_t, NULL
#include "uasm.h"
#include "asm_labels.h" // label_get_offset
#include "strings.h"    // str_to_int
#include "registers.h"  // also includes stdint.h


int assembleNUL(char** items, int n, uint8_t instruction[4])
{
    return 0; // nothing to do
}

int assembleNNN(char** items, int n, uint8_t instruction[4])
{
    if (n != 2) {
        printerror("Command <%s> takes one argument", items[0]);
        return -1;
    }

    long number = 0;

    if (str_to_int(items[1], &number) == -1) {
        printerror("Not a number: <%s>", items[1]);
        return -1;
    }

    instruction[1] = number >> 16;
    instruction[2] = number >> 8;
    instruction[3] = number;
    return 0;
}

int assembleR00(char** items, int n, uint8_t instruction[4])
{
    if (n != 2) {
        printerror("Command <%s> takes one argument", items[0]);
        return -1;
    }

    Register *r = NULL;
    r = get_register_byname(items[1]);
    if (! r) {
        printerror("Not a register: %s", items[1]);
        return -1;
    }
    instruction[1] = r->regno;
    return 0;
}

int assembleRNN(char** items, int n, uint8_t instruction[4])
{
    if ( n != 3) {
        printerror("Command <%s> takes 2 arguments", items[0]);
        return -1;
    }
    
    char *R  = items[1]; // Register name
    char *NN = items[2]; // Number or label

    Register *r = get_register_byname(R);
    if (! r) {
        printerror("Not a register: %s", R);
        return -1;
    }

    long number = 0;
    if (str_to_int(NN, &number) == -1) {
        /* the command argument is not a number
           is it perhaps a label? */
        int offset = 0;
        if (label_get_offset(NN, &offset) == -1) {
            printerror("Undefined label: <%s>", NN);
            return -1;
        } else {            
            number = ITABLE_SIZE + (offset * 4);
        }
    }

    uint16_t num16 = number;
    instruction[1] = r->regno;
    instruction[2] = num16 >> 8;
    instruction[3] = num16;

    return 0;
}

int assembleRR0(char **items, int n, uint8_t instruction[4])
{
    if (n != 3) {
        printerror("Command <%s> takes 2 arguments", items[0]);
        return -1;
    }

    Register *r1 = get_register_byname(items[1]);
    if (! r1) {
        printerror("Not a register: %s", items[1]);
        return -1;
    }
    Register *r2 = get_register_byname(items[2]);
    if (! r2) {
        printerror("Not a register: %s", items[2]);
        return -1;
    }

    instruction[1] = r1->regno;
    instruction[2] = r2->regno;

    return 0;
}

int assembleRRN(char **items, int n, uint8_t instruction[4])
{
    if (n != 4) {
        printerror("Command <%s> takes 3 arguments", items[0]);
        return -1;
    }

    Register *r1 = get_register_byname(items[1]);
    if (! r1) {
        printerror("Not a register: %s", items[1]);
        return -1;
    }
    Register *r2 = get_register_byname(items[2]);
    if (! r2) {
        printerror("Not a register: %s", items[2]);
        return -1;
    }

    long number = 0;
    if (str_to_int(items[3], &number) == -1) {
        printerror("Not a number: <%s>", items[3]);
        return -1;
    }

    uint8_t n8 = number;

    instruction[1] = r1->regno;
    instruction[2] = r2->regno;
    instruction[3] = n8;

    return 0;
}

int assembleRRR(char **items, int n, uint8_t instruction[4])
{
    if (n != 4) {
        printerror("Command <%s> takes 3 register names", items[0]);
        return -1;
    }

    Register *r1 = get_register_byname(items[1]);
    if (! r1) {
        printerror("Not a register: %s", items[1]);
        return -1;
    }

    Register *r2 = get_register_byname(items[2]);
    if (! r2) {
        printerror("Not a register: %s", items[2]);
        return -1;
    }
    Register *r3 = get_register_byname(items[3]);
    if (! r3) {
        printerror("Not a register: %s", items[3]);
        return -1;
    }

    instruction[1] = r1->regno;
    instruction[2] = r2->regno;
    instruction[3] = r3->regno;

    return 0;
}
