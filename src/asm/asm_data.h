#ifndef ASM_DATA_H
#define ASM_DATA_H

#include <stdio.h>
#include <stdint.h>

void insert_data(const char *label, void *data, size_t n);
void delete_data(void);
void write_data(FILE *output);

/** Translate data labels to offsets. 
 * @arg offset the offset of the first data item. 
 *        The offset is given in instructions: how many instructions
 *        where assembled before the data section.
 */
void translate_data_labels(int offset);

#endif