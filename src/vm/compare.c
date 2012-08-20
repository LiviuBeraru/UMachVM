#include <stdio.h>
#include "core.h"
#include "registers.h"

int core_cmp  (void)
{
    int32_t a = 0;
    int32_t b = 0;
    
    if (read_register(instruction[1], &a)) { return -1; }
    if (read_register(instruction[2], &b)) { return -1; }

    if (a == b) {
        registers[CMPR].value =  0;
    } else if (a < b) {
        registers[CMPR].value = -1;
    } else {
        registers[CMPR].value =  1;
    }
    
    return 0;
}

int core_cmpu (void)
{
    printf("%s\n", __func__);
    return 0;
}

int core_cmpi (void)
{
    printf("%s\n", __func__);
    return 0;
}
