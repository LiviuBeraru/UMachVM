/**
 * @file io.c
 * Funtions implementing the I/O commands of the UMach machine.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h> // strlen

#include "core.h"
#include "registers.h"
#include "memory.h"
#include "io.h"
#include "system.h"     // interrupt()
#include "interrupts.h" // interrupt numbers
#include "logmsg.h"

/* specify read or write when calling inout */
#define IO_IN  0
#define IO_OUT 1

/** Help function */
static int inout(int readwrite);


/**
 * IN command of the UMach machine.
 */
int core_in(void)
{
    return inout(IO_IN);
}

int core_out(void)
{
    return inout(IO_OUT);
}

int inout(int readwrite)
{
    /* extract operands from the instruction,
     * these are all register numbers */
    int32_t address_reg = instruction[1];
    int32_t nbytes_reg = instruction[2];
    int32_t inport_reg = instruction[3];

    /* register values; we read these from the machine
     * by using the provided operands */
    int32_t address = 0;
    int32_t nbytes  = 0;
    int32_t inport  = 0;

    /* read register values; while reading a register, the machine might
     * raise an interrupt and return -1, so we also return -1 if reading
     * fails */
    if (read_register(address_reg, &address) == -1) {
        return -1;
    }
    if (read_register(nbytes_reg, &nbytes)   == -1) {
        return -1;
    }
    if (read_register(inport_reg, &inport)   == -1) {
        return -1;
    }

    /* check arguments */
    if (address < 0) {
        logmsg(LOG_WARN, "IO: using negative address %d in register %d",
               address, address_reg);
        interrupt(INT_ILLEGAL_ARG);
        return -1;
    }

    /* nbytes (data length) must be positive */
    if (nbytes < 0) {
        logmsg(LOG_WARN,
               "IO: negative value %d in register %d as length. Programming error?",
               nbytes, nbytes_reg);
        /* we should really have an interrupt for illegal arguments
           now just using 'invalid register' */
        interrupt(INT_ILLEGAL_ARG);
        return -1;
    }

    /* Check the port number */
    if(inport < 0 || inport >= NO_IOPORTS) {
        interrupt(INT_NO_IOPORT);
        return -1;
    }

    /* allocate enough zeroed memory to store the input/output */
    uint8_t *mem = calloc(nbytes + 1, 1); // man calloc
    if (mem == NULL) {
        logmsg(LOG_INFO, "%s: cannot allocate memory of size %d",
            __func__, nbytes + 1);
        interrupt(INT_INTERNAL_ERR);
        return -1;
    }

    int status = 0;

    if (readwrite == IO_IN)
    { // IN
        if (fgets((char *)mem, nbytes + 1, stdin) == NULL) {
            status = -1;
            goto clean;
        }
        if (mem_write(mem, address, nbytes) == -1) {
            status = -1;
            goto clean;
        }
        int n = strlen((char *)mem);
        if (write_register(nbytes_reg, n) == -1) {
            status = -1;
            goto clean;
        }
    } 
    else 
    { // OUT
        if (mem_read(mem, address, nbytes) == -1) {
            // memory read failed
            status = -1;
        } else {
            /* don't care about the port now,
               we send everything to stdout */
            printf("%s", mem);
        }
    }

clean:
    free(mem);
    return status;
}









