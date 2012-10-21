#include <string.h>    // memcmp

#include "registers.h"
#include "core.h"
#include "memory.h"
#include "system.h"     // interrupt()
#include "interrupts.h" // interrupt numbers
#include "logmsg.h"
#include "command.h"

/* Global data */
int     running        = 0;
uint8_t instruction[4] = {0x00, 0x00, 0x00, 0x00};

void core_init(void )
{
    running = 1;
}

void core_run_program(void)
{
    while (running) {
        core_fetch();
        core_execute();
    }
}

void core_fetch(void)
{
    if (! running) {
        logmsg(LOG_WARN, 
        "Core: maschine is not running. Can not fetch.");
        return;
    }
    mem_read(instruction, registers[PC].value, 4);
}

void core_execute(void)
{
    if (! running) {
        logmsg(LOG_ERR,
        "Core: maschine is not running. Can not execute.");
        return;
    }
    
    /* did we hit the data section?
     * if yes, then stop executing
     */
    if (registers[PC].value >= registers[DS].value) {
        logmsg(LOG_INFO, "Core: hit data section. Terminating.");
        running = 0;
        return;
    }
    
    /* the opcode is the first byte of the instruction */
    int opcode = instruction[0];
    struct command *cmd = command_by_opcode(opcode);
    if (cmd != NULL) {
        cmd->opfunc();
    } else {
        logmsg(LOG_ERR, "Core: Unknown opcode: 0x%02X", opcode);
        interrupt(INT_INVALID_CMD);
    }

    registers[PC].value += 4; // next instruction please
    if (registers[PC].value >= mem_getsize()) {
        logmsg(LOG_WARN, 
        "Core: Program Counter greater than memory size. Terminating.");
        interrupt(INT_INVALID_MEM);
    }
    if (registers[PC].value < 0) {
        logmsg(LOG_WARN, "Core: Program Counter is negative. Terminating.");
        interrupt(INT_INVALID_MEM);
    }
}
