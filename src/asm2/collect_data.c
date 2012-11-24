#include <glib.h>
#include <stdlib.h>
#include <string.h>
#include "uasm.h"
#include "collect_data.h"
#include "symbols.h"

static GSList *data_list = NULL;

GSList *get_data_list() {
    return data_list;
}

void insert_string_data(char *label, char *value) {
    data_t *data = malloc(sizeof(data_t));
    
    data->type = DATATYPE_STRING;
    data->string_data.label = malloc(sizeof(char) * (strlen(label) + 1));
    data->string_data.value = malloc(sizeof(char) * (strlen(value) + 1));
    strcpy(data->string_data.label, label);
    strcpy(data->string_data.value, value);

    data_list = g_slist_prepend(data_list, data);
}

void insert_int_data(char *label, int32_t value) {
    data_t *data = malloc(sizeof(data_t));
   
    data->type = DATATYPE_INT;
    data->int_data.label = malloc(sizeof(char) * (strlen(label) + 1));
    strcpy(data->int_data.label, label);
    data->int_data.value = value;

    data_list = g_slist_prepend(data_list, data);
}

int insert_data_symbols(asm_context_t *cntxt) {
    data_list = g_slist_reverse(data_list);

    for (GSList *l = data_list; l != NULL; l = g_slist_next(l)) {
        data_t *data = l->data;

        symbol_t *sym = malloc(sizeof(symbol_t));
        
        switch (data->type) {
        case DATATYPE_STRING:
            sym->sym_name = malloc(sizeof(char) * (strlen(data->string_data.label) + 1));
            strcpy(sym->sym_name, data->string_data.label);
            sym->sym_type = SYMTYPE_STRDAT;
            sym->sym_addr = cntxt->current_addr;

            if (!insert_symbol(sym)) {
                print_error(cntxt, "String constant %s already defined", sym->sym_name);
                free(sym->sym_name);
                free(sym);
                return FALSE;
            }

            // find next ALIGNED addr
            cntxt->current_addr += (strlen(data->string_data.value) + 1);
            while (cntxt->current_addr % 4 != 0)
                cntxt->current_addr++;
                
            break;
            
        case DATATYPE_INT:
            sym->sym_name = malloc(sizeof(char) * (strlen(data->int_data.label) + 1));
            strcpy(sym->sym_name, data->int_data.label);
            sym->sym_type = SYMTYPE_INTDAT;
            sym->sym_addr = cntxt->current_addr;

            if (!insert_symbol(sym)) {
                print_error(cntxt, "Integer constant %s already defined", sym->sym_name);
                free(sym->sym_name);
                free(sym);
                return FALSE;
            }

            cntxt->current_addr += 4;
            break;
        }
    }

    return TRUE;
}

static void free_data_helper(gpointer data) {
    data_t *d = data;

    switch (d->type) {
    case DATATYPE_INT:
        free(d->int_data.label);
        free(d);
        break;
    case DATATYPE_STRING:
        free(d->string_data.label);
        free(d->string_data.value);
        free(d);
        break;
    }
}

void free_data() {
    g_slist_free_full(data_list, free_data_helper);
    data_list = NULL;
}
