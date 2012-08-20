#include <stdio.h>
#include <stdlib.h>

#include "core.h"
#include "registers.h"
#include "memory.h"

int core_in  (void)
{
    printf("%s\n", __func__);
    return 0;
}

int core_out (void)
{
    int32_t address = 0;
    int32_t nbytes = 0;
    int32_t port = 0;
    
    /* read values from registers */
    if (read_register(instruction[1], &address)) { return -1; }
    if (read_register(instruction[2], &nbytes))  { return -1; }
    if (read_register(instruction[3], &port))    { return -1; }
    
    uint8_t *mem = calloc(nbytes + 1, 1); // +1 for '\0'
    if (mem_read(mem, address, nbytes) == -1) {
        return -1;
    }
    
    /* don't care about the port now, 
       we send everything to stdout */
    printf("%s", mem);
    free(mem);
    
    return 0;
}
