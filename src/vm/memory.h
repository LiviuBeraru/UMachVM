#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>
#include <stddef.h> // size_t

/** Interrupt table size. 
 *  For 64 interrupt entries we have 64*4 byte = 256 bytes. */
#define ITABLE_SIZE 256


int    mem_init(size_t bytes);
int    mem_load_program_file(const char *filename);
void   mem_free(void);
int    mem_read(uint8_t *destination, int index, int nbytes);
int    mem_write(uint8_t *source, int index, int nbytes);
int    mem_push(int32_t word);
int    mem_pop(int32_t *word);
size_t mem_getsize(void);

/**
 * Convert the nbytes-integer stored in big-endian order
 * at memory address mem to a 4-integer. The sign of the integer at mem
 * is preserved.
 */
int32_t mem_to_int(const uint8_t *mem, int nbytes);


/** This 4 byte 'instruction' marks the begin of the data section of a programm. 
 *  After this instruction, no executable commands are to be found. */
extern uint8_t begin_data[4];

#endif
