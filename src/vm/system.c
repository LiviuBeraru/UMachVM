#include "core.h"
#include "logmsg.h"

int interrupt(int number)
{
    logmsg(LOG_WARN, "Interrupt %d", number);
    running = 0;
    return 0;
}

int core_int(void)
{
    int8_t b = instruction[1];
    int ino = (b << 16) | (instruction[2] << 8) | (instruction[3]);
    return (interrupt(ino));
}


