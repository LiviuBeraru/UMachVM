#ifndef ASM_DATA_H
#define ASM_DATA_H

#include <stdio.h>

void insert_data(const char *label, void *data, size_t n);
void delete_data(void);
void write_data(FILE *output);
void insert_data_labels(int offset);

#endif
