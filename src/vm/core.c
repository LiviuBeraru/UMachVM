#include <stdio.h>

#include "registers.h"
#include "core.h"
#include "memory.h"
#include "system.h"
#include "interrupts.h"
#include "logmsg.h"
#include "command.h"

static Register  registers[NOREGS];
static State     state = {1, registers};

static void init_registers(void);

void core_init(void)
{
    init_registers();
}

void init_registers(void)
{
    int i;
    for (i = 1; i <= 32; i++) {
        registers[i].mode = (REG_RW);
    }
    registers[PC].value = 256;
    registers[PC].mode = REG_READABLE;
    
    registers[SP].value = mem_getsize();
    registers[SP].mode = REG_RW;
    
    registers[FP].value = mem_getsize();
    registers[FP].mode  = REG_RW;
    registers[IR].mode  = REG_RW;
    registers[STAT].mode = REG_RW;
    registers[ERR].mode = REG_RW;
    registers[HI].mode = REG_READABLE;
    registers[LO].mode = REG_READABLE;    
    registers[CMPR].mode = REG_READABLE;
    registers[ZERO].mode = REG_READABLE;
}

void core_run_program(void)
{
    while (state.running) {
        core_fetch();
        core_execute();
    }
}

void core_fetch(void)
{
    mem_read(state.instruction, state.registers[PC].value, 4);
}

void core_execute(void)
{
    /* the opcode is the first byte of the instruction */
    int opcode = state.instruction[0];
    struct command *cmd;
    cmd = command_by_opcode(opcode);
    if (cmd != NULL) {
        cmd->opfunc();
    } else {
        logmsg(LOG_ERR, "Core: Unknown opcode: 0x%02X", opcode);
        interrupt(INT_INVALID_CMD);
    }

    state.registers[PC].value += 4;
    if (state.registers[PC].value >= mem_getsize()) {
        state.running = 0;
        logmsg(LOG_WARN, 
        "Core: Program Counter greater than memory size. Terminating.");
    }
}

State* core_getstate(void)
{
    return &state;
}

void core_dump_regs(void)
{
    int i = 1;
    for (i = 1; i <= 32; i++) {
        printf("| R%-2d = %3d ", i, registers[i].value);
        if (i % 8 == 0) {
            printf("|\n");
        }
    }
}
