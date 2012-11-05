#include <glib.h>
#include <stdlib.h>
#include <string.h>
#include "uasm.h"
#include "collect_data.h"
#include "symbols.h"

static GSList *str_data_list = NULL;
static GSList *int_data_list = NULL;

GSList *get_int_data_list() {
    return int_data_list;
}

GSList *get_string_data_list() {
    return str_data_list;
}

void insert_string_data(char *label, char *value) {
	string_data_t *data = malloc(sizeof(string_data_t));
	
	data->label = malloc(sizeof(char) * (strlen(label) + 1));
	data->value = malloc(sizeof(char) * (strlen(value) + 1));
	strcpy(data->label, label);
	strcpy(data->value, value);

    str_data_list = g_slist_prepend(str_data_list, data);
}

void insert_int_data(char *label, int32_t value) {
	int_data_t *data = malloc(sizeof(int_data_t));
	
	data->label = malloc(sizeof(char) * (strlen(label) + 1));
	
	strcpy(data->label, label);
	data->value = value;

	int_data_list = g_slist_prepend(int_data_list, data);
}

void insert_data_symbols(asm_context_t *cntxt) {
    str_data_list = g_slist_reverse(str_data_list);
    int_data_list = g_slist_reverse(int_data_list);

    for (GSList *l = int_data_list; l != NULL; l = g_slist_next(l)) {
        int_data_t *data = l->data;

        symbol_t *sym = malloc(sizeof(symbol_t));

        sym->symname = malloc(sizeof(char) * (strlen(data->label) + 1));
        strcpy(sym->symname, data->label);
        sym->symtype = SYMTYPE_DATA;
        sym->symaddr = cntxt->current_addr;

        insert_symbol(sym);

        cntxt->current_addr += 4;
    }

    for (GSList *l = str_data_list; l != NULL; l = g_slist_next(l)) {
        string_data_t *data = l->data;

        symbol_t *sym = malloc(sizeof(symbol_t));

        sym->symname = malloc(sizeof(char) * (strlen(data->label) + 1));
        strcpy(sym->symname, data->label);
        sym->symtype = SYMTYPE_DATA;
        sym->symaddr = cntxt->current_addr;

        insert_symbol(sym);

        // find next ALIGNED addr
        cntxt->current_addr += (strlen(data->value) + 1);
        while (cntxt->current_addr % 4 != 0)
            cntxt->current_addr++;
    }
}
