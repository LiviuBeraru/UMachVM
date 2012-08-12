#include "core.h"
#include "registers.h"

static int regno_invalid(int regno)
{
    if (regno < 0 || regno > NOREGS) {
        return -1;
    }
    
    return 0;
}

static int reg_cant(int regno, int mode)
{
    State *state = core_getstate();
    if (regno_invalid(regno)) {
        return -1;
    }
    Register r = state->registers[regno];
    if (r.mode & mode) {
        return 0;
    } else {
        return -1;
    }
}

int reg_noread(int regno)
{
    return reg_cant(regno, REG_READABLE);
}

int reg_nowrite(int regno)
{
    return reg_cant(regno, REG_WRITABLE);
}

