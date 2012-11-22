#ifndef UASM_H
#define UASM_H

#include <stdint.h>

#define INTTABLE_SIZE 256

typedef struct {
    uint32_t current_addr;
    uint32_t current_line;
    uint32_t current_f_id;
    char    *current_file;
    char     *output_file;
    char     gen_debuginf;
} asm_context_t;

void print_error(asm_context_t *cntxt, const char *format, ...);

#endif
