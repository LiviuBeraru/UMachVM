#ifndef ASM_DATA_H
#define ASM_DATA_H

#include <stdio.h>
#include <stdint.h>

void insert_data(const char *label, void *data, size_t n);
void delete_data(void);
void write_data(FILE *output);

/** Translate labels to offsets. 
 * @arg offset the offset of the first data item. 
 *        The offset is given in instructions: how many instructions
 *        where assembled before the data section.
 */
void translate_labels(int offset);

/** This 4 byte 'instruction' marks the begin of the data section of a programm. 
 *  After this instruction, no executable commands are to be found. */
extern uint8_t begin_data[4];

#endif
