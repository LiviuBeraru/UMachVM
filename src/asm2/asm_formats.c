/*
 * asm_formats.c
 * 
 * translate command operands from string to bytecode format
 * 
 */

#include <glib.h>
#include <stddef.h>     // size_t, NULL
#include "uasm.h"
#include "str_func.h"   // str_to_int
#include "registers.h"  // also includes stdint.h
#include "symbols.h"
#include "commands.h"

int assemble_nul(asm_context_t *cntxt, char *items[], int n, uint8_t instruction[4]) {
    // "items" is formally required, but we don't need it.
    // Let's keep the compiler silent!
    (void) items;

    if (n != 1) {
        print_error(cntxt, "Command <%s> expects %s", items[0], get_cmdfmt_name(CMDFMT_NUL));
        return FALSE;
    }

    instruction[1] = 0x00;
    instruction[2] = 0x00;
    instruction[3] = 0x00;
    return TRUE;
}

int assemble_nnn(asm_context_t *cntxt, char *items[], int n, uint8_t instruction[4]) {
    if (n != 2) {
        print_error(cntxt, "Command <%s> expects %s", items[0], get_cmdfmt_name(CMDFMT_NNN));
        return FALSE;
    }

    long number = 0;

    if (!str_to_int(items[1], &number)) {
        print_error(cntxt, "Not a number: <%s>", items[1]);
        return FALSE;
    }

    instruction[1] = number >> 16;
    instruction[2] = number >> 8;
    instruction[3] = number >> 0;

    return TRUE;
}

int assemble_r00(asm_context_t *cntxt, char *items[], int n, uint8_t instruction[4]) {
    if (n != 2) {
        print_error(cntxt, "Command <%s> expects %s", items[0], get_cmdfmt_name(CMDFMT_R00));
        return FALSE;
    }

    register_t r;

    if (!get_register_by_name(items[1], &r)) {
        print_error(cntxt, "Not a register: <%s>", items[1]);
        return FALSE;
    }

    instruction[1] = r.regcode;
    instruction[2] = 0x00;
    instruction[3] = 0x00;

    return TRUE;
}

int assemble_rnn(asm_context_t *cntxt, char *items[], int n, uint8_t instruction[4]) {
    if ( n != 3) {
        print_error(cntxt, "Command <%s> expects %s", items[0], get_cmdfmt_name(CMDFMT_RNN));
        return FALSE;
    }
    
    char *nn_part = items[2]; // number or label

    register_t r;

    if (!get_register_by_name(items[1], &r)) {
        print_error(cntxt, "Not a register: <%s>", items[1]);
        return FALSE;
    }

    long number = 0;

    if (!str_to_int(nn_part, &number)) {
        /* the command argument is not a number
         * check whether it is a label */
        symbol_t label;

        if (get_symbol(nn_part, &label)) {
            number = label.sym_addr;
        } else {            
            print_error(cntxt, "Undefined label: <%s>", nn_part);
            return FALSE;
        }
    }

    uint16_t num16 = number;
    instruction[1] = r.regcode;
    instruction[2] = num16 >> 8;
    instruction[3] = num16 >> 0;

    return TRUE;
}

int assemble_rr0(asm_context_t *cntxt, char *items[], int n, uint8_t instruction[4]) {
    if (n != 3) {
        print_error(cntxt, "Command <%s> expects %s", items[0], get_cmdfmt_name(CMDFMT_RR0));
        return FALSE;
    }

    register_t r1, r2;

    if (!get_register_by_name(items[1], &r1)) {
        print_error(cntxt, "Not a register: <%s>", items[1]);
        return FALSE;
    }

    if (!get_register_by_name(items[2], &r2)) {
        print_error(cntxt, "Not a register: <%s>", items[2]);
        return FALSE;
    }

    instruction[1] = r1.regcode;
    instruction[2] = r2.regcode;
    instruction[3] = 0x00;

    return TRUE;
}

int assemble_rrn(asm_context_t *cntxt, char *items[], int n, uint8_t instruction[4]) {
    if (n != 4) {
        print_error(cntxt, "Command <%s> expects %s", items[0], get_cmdfmt_name(CMDFMT_RRN));
        return FALSE;
    }

    register_t r1, r2;

    if (!get_register_by_name(items[1], &r1)) {
        print_error(cntxt, "Not a register: <%s>", items[1]);
        return FALSE;
    }

    if (!get_register_by_name(items[2], &r2)) {
        print_error(cntxt, "Not a register: <%s>", items[2]);
        return FALSE;
    }

    long number = 0;

    if (!str_to_int(items[3], &number)) {
        print_error(cntxt, "Not a number: <%s>", items[3]);
        return FALSE;
    }

    instruction[1] = r1.regcode;
    instruction[2] = r2.regcode;
    instruction[3] = (uint8_t) number;

    return TRUE;
}

int assemble_rrr(asm_context_t *cntxt, char *items[], int n, uint8_t instruction[4]) {
    if (n != 4) {
        print_error(cntxt, "Command <%s> expects %s", items[0], get_cmdfmt_name(CMDFMT_RRR));
        return FALSE;
    }

    register_t r1, r2, r3;

    if (!get_register_by_name(items[1], &r1)) {
        print_error(cntxt, "Not a register: <%s>", items[1]);
        return FALSE;
    }

    if (!get_register_by_name(items[2], &r2)) {
        print_error(cntxt, "Not a register: <%s>", items[2]);
        return FALSE;
    }

    if (!get_register_by_name(items[3], &r3)) {
        print_error(cntxt, "Not a register: <%s>", items[3]);
        return FALSE;
    }

    instruction[1] = r1.regcode;
    instruction[2] = r2.regcode;
    instruction[3] = r3.regcode;

    return TRUE;
}
