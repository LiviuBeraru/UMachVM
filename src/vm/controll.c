#include "core.h"

int core_nop(void)
{
    // do nothing
    return 0;
}

int core_eop(void)
{
    // stop the machine
    running = 0;
    return 0;
}
