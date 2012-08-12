#include <stddef.h> // NULL
#include "core.h"

int core_nop(void)
{
    // do nothing
    return 0;
}

int core_eop(void)
{
    // stop the machine
    State *state = core_getstate();
    if (state != NULL) {
        state->running = 0;
    }
    return 0;
}
