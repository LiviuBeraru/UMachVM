#ifndef COLLECT_DATA_H
#define COLLECT_DATA_H

#include <stdint.h>

typedef enum {
    DATATYPE_INT,
    DATATYPE_STRING
} data_type_t;

typedef struct {
    char *label;
    char *value;
} string_data_t;

typedef struct {
    char   *label;
    int32_t value;
} int_data_t;

typedef struct {
    data_type_t type;
    union {
        string_data_t string_data;
        int_data_t    int_data;
    };
} data_t;

GSList *get_data_list();
void insert_string_data(char *label, char *value);
void insert_int_data(char *label, int32_t value);
int insert_data_symbols(asm_context_t *cntxt);
void free_data();

#endif
