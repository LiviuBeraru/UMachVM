#include <stdio.h>
#include <stdlib.h>
#include <string.h> //memcpy
#include "memory.h"
#include "logmsg.h"
#include "system.h"
#include "interrupts.h"

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

    fread(memory + ITABLE_SIZE, 1, fsize, file);

    fclose(file);
    return fsize;
}

int mem_read(uint8_t *destination, int index, int nbytes)
{
    if (memory == NULL) {
        logmsg(LOG_ERR, "Cannot read from NULL memory");
        interrupt(INT_INTERNAL_ERR);
        return -1;
    }

    if (index < 0 || index >= memsize) {
        logmsg(LOG_ERR, "Illegal memory index: %d", index);
        interrupt(INT_INVALID_MEM);
        return -1;
    }

    if ((index + nbytes) > memsize) {
        logmsg(LOG_ERR, "Cannot read %d bytes from memory: too much", nbytes);
        interrupt(INT_INVALID_MEM);
        return -1;
    }

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
        logmsg(LOG_ERR, "Cannot write to NULL memory");
        interrupt(INT_INTERNAL_ERR);
        return -1;
    }

    if (index < 0 || index >= memsize) {
        logmsg(LOG_ERR, "Illegal memory index: %d", index);
        interrupt(INT_INVALID_MEM);
        return -1;
    }

    if ((index + nbytes) > memsize) {
        logmsg(LOG_ERR, "Cannot write %d bytes to memory: too much", nbytes);
        interrupt(INT_INVALID_MEM);
        return -1;
    }

    memcpy(memory+index, source, nbytes);
    return 0;
}


size_t mem_getsize(void)
{
    return memsize;
}
