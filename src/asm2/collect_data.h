#ifndef COLLECT_DATA_H
#define COLLECT_DATA_H

#include <stdint.h>

typedef struct {
    char *label;
    char *value;
} string_data_t;

typedef struct {
    char   *label;
    int32_t value;
} int_data_t;

GSList *get_int_data_list();
GSList *get_string_data_list();
void insert_string_data(char *label, char *value);
void insert_int_data(char *label, int32_t value);
int insert_data_symbols(asm_context_t *cntxt);

#endif
