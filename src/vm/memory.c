#include <stdio.h>
#include <stdlib.h>
#include <string.h> //memcpy
#include "memory.h"
#include "logmsg.h"
#include "system.h"
#include "interrupts.h"
#include "registers.h"

static int8_t *memory = NULL;
static size_t memsize = 0;

int mem_init(size_t bytes)
{
    if (bytes < (ITABLE_SIZE + 4)) {
        logmsg(LOG_ERR, "Memory must be at least %d+4 bytes big.", ITABLE_SIZE);
        return -1;
    }

    if (memory != NULL) {
        free(memory);
    }

    memory = calloc(bytes, sizeof(*memory));

    if (memory == NULL) {
        logmsg(LOG_ERR, "%s: cannot allocate %u bytes of memory", bytes);
        return -1;
    }

    memsize = bytes;
    return bytes;
}

void mem_free(void)
{
    free(memory);
    memory = NULL;
    memsize = 0;
}

int mem_load_program_file(const char* filename)
{
    if (memory == NULL) {
        logmsg(LOG_ERR, "Cannot load program: memory not initialized.");
        return -1;
    }

    FILE *file = fopen(filename, "r");
    if(file == NULL) {
        perror(filename);
        return -1;
    }

    /* get the file size in bytes */
    fseek(file, 0L, SEEK_END);
    long fsize = ftell(file);
    fseek(file, 0L, SEEK_SET);

    /* check if we have enought memory to store the program file.
       the memory must be big enough to store at least the interrupt
       table and the program */
    if (ITABLE_SIZE + fsize >= memsize) {
        logmsg(LOG_ERR,
               "Cannot load %ld bytes of program into %d bytes of memory",
               fsize, memsize);
        fclose(file);
        return -1;
    }

    //TODO: load 4 byte chunks and check for the data mark
    fread(memory + ITABLE_SIZE, 1, fsize, file);

    fclose(file);
    return fsize;
}

int mem_read(uint8_t *destination, int index, int nbytes)
{
    if (memory == NULL) {
        /* memory was not initialized */
        logmsg(LOG_ERR, "Cannot read from NULL memory");
        interrupt(INT_INTERNAL_ERR);
        return -1;
    }

    if (index < 0 || index >= memsize) {
        /* invalid memory index */
        logmsg(LOG_ERR, "Illegal memory index: %d", index);
        interrupt(INT_INVALID_MEM);
        return -1;
    }

    if ((index + nbytes) > memsize) {
        /* invalid number of bytes to read */
        logmsg(LOG_ERR, "Cannot read %d bytes from memory: too much", nbytes);
        interrupt(INT_INVALID_MEM);
        return -1;
    }
    
    /* We consider all memory to be readable, 
     * so no other checks are made */

    memcpy(destination, memory+index, nbytes);
    return 0;
}

/** Writes nbytes bytes from source into memory beginning
    from the specified index.
    This function will generate interrups.
    */
int mem_write(uint8_t* source, int index, int nbytes)
{
    if (memory == NULL) {
        /* memory was not previously initialized */
        logmsg(LOG_ERR, "Cannot write to NULL memory");
        interrupt(INT_INTERNAL_ERR);
        return -1;
    }

    if (index < 0 || index >= memsize) {
        /* wrong memory index (memory address) */
        logmsg(LOG_ERR, "Illegal memory index: %d", index);
        interrupt(INT_INVALID_MEM);
        return -1;
    }
    
    //TODO: check for read only code segment

    if ((index + nbytes) > memsize) {
        logmsg(LOG_ERR, "Cannot write %d bytes to memory: too much", nbytes);
        interrupt(INT_INVALID_MEM);
        return -1;
    }

    memcpy(memory+index, source, nbytes);
    return 0;
}

int mem_push(int32_t word) 
{
    /* turn the 32 bit word into a 4 byte buffer according 
     * to the big endian notation.
     * we could also use the posix function htonl
     */
    uint8_t buffer[4] = { 0x0 };
    buffer[0] = word >> 24;
    buffer[1] = word >> 16;
    buffer[2] = word >>  8;
    buffer[3] = word;

    registers[SP].value -= 4;
    if (registers[SP].value < 0) {
        //TODO: check  '< 0' to '< HE.value' (HE register) 
        
        /* We generate a stack overflow interrupt if the 
         * SP register has a value less than zero. Future versions
         * should generate an interrupt if the SP register points
         * to a memory region which is occupied by the programm itself.
         * To do this one should store the program size in the memory.c module.
         */
        logmsg(LOG_WARN, "Mem: Stack Overflow: cannot PUSH");
        registers[SP].value += 4; // reset SP
        interrupt(INT_STACK_OVERFLOW);
        return -1;
    }
    
    return (mem_write(buffer, registers[SP].value, 4));
}

int mem_pop(int32_t* word)
{
    uint8_t buffer[4] = { 0x0 };
    if (mem_read(buffer, registers[SP].value, 4) == -1) {
        logmsg(LOG_WARN, "Mem: Stack Error: cannot POP");
        interrupt(INT_INVALID_MEM);
        return -1;
    }
    
    *word = (buffer[0] << 24) | 
            (buffer[1] << 16) | 
            (buffer[2] <<  8) | 
             buffer[3];
    
    registers[SP].value += 4;
    return 0;
}


size_t mem_getsize(void)
{
    return memsize;
}
