#ifndef SYMBOLS_H
#define SYMBOLS_H

#include <stdint.h>

typedef enum {
    SYMTYPE_JUMP,
    SYMTYPE_DATA
} symbol_type_t;

typedef struct {
    char         *symname;
    symbol_type_t symtype;
    uint32_t      symaddr;
} symbol_t;

int  insert_symbol(symbol_t *symbol);
int  get_symbol(const char *name, symbol_t *result);
void free_symbols();
void write_symbols_file();

#endif
