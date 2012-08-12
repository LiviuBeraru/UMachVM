#include "core.h"
#include "logmsg.h"

int interrupt(int number)
{
    logmsg(LOG_WARN, "Interrupt %d", number);

    State *state = core_getstate();
    if (state != NULL)
    {
        state -> running = 0;
    }
    return 0;
}

int core_int(void)
{
    State *state = core_getstate();
    int ino = state->instruction[1];
    ino = (ino << 8) | state->instruction[2];
    ino = (ino << 8) | state->instruction[3];

    return (interrupt(ino));
}


