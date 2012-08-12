#ifndef MEMORY_H
#define MEMORY_H

#include <stdio.h> // FILE*
#include <stdint.h>

/** Interrupt table size. 
 *  For 64 interrupt entries we have 64*4 byte = 256 bytes. */
#define ITABLE_SIZE 256


int    mem_init(size_t bytes);
int    mem_load_program_file(FILE *file);
void   mem_free(void);
int    mem_read(uint8_t *destination, int index, int nbytes);
int    mem_write(uint8_t *source, int index, int nbytes);
size_t mem_getsize(void);

#endif
