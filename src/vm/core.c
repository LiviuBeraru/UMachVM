#include <stdio.h> // TODO: remove this

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
    if (mem_getsize() == 0) {
        logmsg(LOG_ERR, "Core: memory not initialized.");
        interrupt(INT_INTERNAL_ERR);
        return;
    }
    registers[SP].value = mem_getsize();
    registers[FP].value = mem_getsize();
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
        logmsg(LOG_WARN, "Core: maschine is not running. Cannot fetch.");
        return;
    }
    mem_read(instruction, registers[PC].value, 4);
}

void core_execute(void)
{
    if (! running) {
        logmsg(LOG_ERR, "Core: maschine is not running. Cannot execute.");
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

    registers[PC].value += 4;
    if (registers[PC].value >= mem_getsize()) {
        running = 0;
        logmsg(LOG_WARN, 
        "Core: Program Counter greater than memory size. Terminating.");
    }
}

//TODO: move this to the debugger and delete #include <stdio.h>
void core_dump_regs(void)
{
    int i = 1;
    for (i = 1; i <= 32; i++) {
        printf("| R%-2d = %5d ", i, registers[i].value);
        if (i % 4 == 0) {
            printf("|\n");
        }
    }
}
