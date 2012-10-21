#include <stdio.h>
#include <stdlib.h>
#include <string.h> //memcpy
#include "memory.h"
#include "logmsg.h"
#include "system.h"
#include "interrupts.h"
#include "registers.h"

static uint8_t *memory = NULL;
static size_t memsize = 0;

uint8_t begin_data[4] = {0xFF, 'D', 'A', 'T'};


int mem_init(size_t bytes)
{
    if (bytes < (ITABLE_SIZE + 4)) {
        logmsg(LOG_ERR, "Mem: Memory must be at least %d+4 bytes big.", 
               ITABLE_SIZE);
        return -1;
    }

    if (memory != NULL) {
        free(memory);
    }

    memory = calloc(bytes, sizeof(*memory));

    if (memory == NULL) {
        logmsg(LOG_ERR, "Mem: cannot allocate %u bytes of memory", bytes);
        return -1;
    }

    memsize = bytes;
    registers[SP].value = memsize;
    registers[FP].value = memsize;
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
        /* must call mem_init first */
        logmsg(LOG_ERR, "Mem: Cannot load program: memory not initialized.");
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
               "Mem: Cannot load %ld bytes of program into %d bytes of memory",
               fsize, mem_getsize());
        fclose(file);
        return -1;
    }

    uint8_t buffer[4] = { 0 };
    uint8_t *mindex = memory + ITABLE_SIZE;
    while( fread(buffer, 1, 4, file) > 0 ) {
        
        if (memcmp(buffer, begin_data, 4) == 0) {
            /* set the DS register to the memory address where the last
             * read word (the data marker) would have been stored. 
             * we dont't store the data marker itself into memory because it
             * is either code nor data.
             */
            registers[DS].value = mindex - memory;
            logmsg(LOG_INFO, "Mem: loading file: set DS = %d", registers[DS].value);            
        } else {
            memcpy(mindex, buffer, 4 );
            mindex += 4;
        }
        
        memset(buffer, 0 , 4);
    }
    
    /* Heap section follows just after the data section */
    registers[HS].value = mindex - memory;
    /* End of heap is defaulted to the heap begin */
    registers[HE].value = registers[HS].value;

    fclose(file);
    return fsize;
}

int mem_read(uint8_t* destination, int index, int nbytes)
{
    if (memory == NULL) {
        /* memory was not initialized */
        logmsg(LOG_ERR, "Mem: Cannot read from NULL memory");
        interrupt(INT_INTERNAL_ERR);
        return -1;
    }

    if (index < 0 || index >= memsize) {
        /* invalid memory index */
        logmsg(LOG_ERR, "Mem: Illegal memory index: %d", index);
        interrupt(INT_INVALID_MEM);
        return -1;
    }

    if ((index + nbytes) > memsize) {
        /* invalid number of bytes to read */
        logmsg(LOG_ERR, "Mem: Cannot read %d bytes from memory: too much", nbytes);
        interrupt(INT_INVALID_MEM);
        return -1;
    }
    
    /* We consider all memory to be readable, 
     * so no further checks are made */

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
        logmsg(LOG_ERR, "Mem: Cannot write to NULL memory");
        interrupt(INT_INTERNAL_ERR);
        return -1;
    }

    // check index range
    if (index < 0 || index >= memsize) {
        /* wrong memory index (memory address) */
        logmsg(LOG_ERR, "Mem: Illegal memory index: %d", index);
        interrupt(INT_INVALID_MEM);
        return -1;
    }

    if ((index + nbytes) > memsize) {
        logmsg(LOG_ERR, "Mem: Cannot write %d bytes to memory: too much", nbytes);
        interrupt(INT_INVALID_MEM);
        return -1;
    }
    
    // check segmentation fault
    if (ITABLE_SIZE <= index && index < registers[DS].value) {
        /* index points to the code segment between 
         * the interrupt table and the data segment */
        interrupt(INT_SEGFAULT);
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
    if (registers[SP].value <= registers[HE].value) {
        /* We generate a stack overflow interrupt if the 
         * SP register has a value less or equal than the HE register,
         * which stores the address of the last byte on the heap.
         */
        logmsg(LOG_WARN, "Mem: Stack Overflow: can not PUSH");
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
    
    *word = mem_to_int(buffer, 4);
    
    registers[SP].value += 4;
    return 0;
}


size_t mem_getsize(void)
{
    return memsize;
}

int32_t mem_to_int(const uint8_t* mem, int nbytes)
{
    int32_t result = 0;
    uint8_t byte   = 0; // unsigned to remove leading sign bits
    int shift = 24;
    int i;

    for (i = 0; i < nbytes; i++, shift = shift - 8) {
        byte = mem[i];
        result = result | (byte << shift);
    }

    result = result / (1 << ((4 - nbytes) * 8));

    return result;
}
