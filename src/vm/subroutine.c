#include "memory.h"
#include "registers.h" // register names
#include "branch.h"    // core_jmp


#include <stdio.h>


int core_go(void)
{
    printf("%s\n", __func__);
    return 0;
}

// call is like branching but does more
int core_call(void)
{
    /* save the current PC */
    if (mem_push(registers[PC].value) == -1) { return -1; }
    if (core_jmp() == -1) { return -1; }

    return 0;
}

int core_ret(void)
{
    int32_t pc = 0;
    if (mem_pop(&pc) == -1) { return -1; }
    
    /* restore the last saved PC */
    registers[PC].value = pc;
    
    return 0;
}
