#include <glib.h>
#include <stdlib.h>
#include "symbols.h"

static GHashTable *symbols_ht = NULL;

static void init_symbols_ht() {
	symbols_ht = g_hash_table_new(g_str_hash, g_str_equal);
}

int insert_symbol(symbol_t *symbol) {
	if (symbols_ht == NULL)
		init_symbols_ht();

    if (g_hash_table_contains(symbols_ht, symbol->symname))
        return FALSE;

	g_hash_table_insert(symbols_ht, symbol->symname, symbol);
    return TRUE;
}

int get_symbol(const char *name, symbol_t *result) {
	if (symbols_ht == NULL)
		return FALSE;
		
	symbol_t *sym = g_hash_table_lookup(symbols_ht, name);
	
	if (sym != NULL) {
		result->symname = sym->symname;
		result->symtype = sym->symtype;
		result->symaddr = sym->symaddr;
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

    free(sym->symname);
    free(sym);

    return TRUE;
}

void free_symbols() {
    g_hash_table_foreach_remove(symbols_ht, free_symbol_helper, NULL);
}
