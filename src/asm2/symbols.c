/*
 * symbols.c
 * 
 * functions to manage and query the symbol table
 * 
 */

#include <glib.h>
#include <stdlib.h>
#include <stdio.h>
#include "symbols.h"

static GHashTable *symbols_ht = NULL;

static void init_symbols_ht() {
    symbols_ht = g_hash_table_new(g_str_hash, g_str_equal);
}

int insert_symbol(symbol_t *symbol) {
    if (symbols_ht == NULL)
        init_symbols_ht();

    if (g_hash_table_contains(symbols_ht, symbol->sym_name))
        return FALSE;

    g_hash_table_insert(symbols_ht, symbol->sym_name, symbol);
    return TRUE;
}

int get_symbol(const char *name, symbol_t *result) {
    if (symbols_ht == NULL)
        return FALSE;
        
    symbol_t *sym = g_hash_table_lookup(symbols_ht, name);
    
    if (sym != NULL) {
        result->sym_name = sym->sym_name;
        result->sym_type = sym->sym_type;
        result->sym_addr = sym->sym_addr;
        return TRUE;
    } else
        return FALSE;
}

static gboolean free_symbol_helper(gpointer key, gpointer value, gpointer user_data) {
    symbol_t *sym = value;

    // "key" & "user_data" are formally required, but we don't need them.
    // Let's keep the compiler silent!
    (void) key;
    (void) user_data;

    free(sym->sym_name);
    free(sym);

    return TRUE;
}

void free_symbols() {
    if (symbols_ht == NULL)
        return;

    g_hash_table_foreach_remove(symbols_ht, free_symbol_helper, NULL);
    g_hash_table_destroy(symbols_ht);
    symbols_ht = NULL;
}

// creates the symbol file used by the debugger
void write_symbols_file(FILE *f) {
    if (symbols_ht == NULL)
        return;

    GHashTableIter iter;
    gpointer key, value;

    g_hash_table_iter_init(&iter, symbols_ht);
    
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        symbol_t *sym = value;

        switch (sym->sym_type) {
        case SYMTYPE_JUMP:
            fprintf(f, "%08x jmp %s\n", sym->sym_addr, sym->sym_name);
            break;
        case SYMTYPE_INTDAT:
            fprintf(f, "%08x int %s\n", sym->sym_addr, sym->sym_name);
            break;
        case SYMTYPE_STRDAT:
            fprintf(f, "%08x str %s\n", sym->sym_addr, sym->sym_name);
            break;
        } 
    }
}
