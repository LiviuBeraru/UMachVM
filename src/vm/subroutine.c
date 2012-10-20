#include "memory.h"
#include "registers.h"  // register names
#include "branch.h"     // core_jmp
#include "core.h"       // instruction[4]
#include "system.h"     // interrupt()
#include "interrupts.h" // interrupt numbers

/**
 * Implements the GO command.
 */
int core_go(void)
{
    /* register number is the second byte of the instruction, 
     * after the opcode */
    int32_t regno = instruction[1];
    int32_t regval = 0;
    /* try to read the register with number regno */
    if (read_register(regno, &regval) == -1) { return -1; }
    
    /* check if the register value contains an address to executable code.
     * executable code (the code segment) is the memory chunk between 
     * address 0 and the begin of the data segment, which is stored in the 
     * register DS. If the register value contains an address outside of the
     * code segment, we interrupt with segmentation fault.
     */
    
    if (regval < 0 || regval >= registers[DS].value) {
        interrupt(INT_SEGFAULT);
        return -1;
    }
    
    /* set the PC register to the register value */
    registers[PC].value = regval;
    
    /* because the machine adds 4 to the PC after executing this
     * instruction, we must adjust the value by subtracting 4
     */
    registers[PC].value -= 4;
    
    return 0;
}

/** 
 * Implemets the CALL command.
 * Pushes the current PC value and the calls JMP.
 */
int core_call(void)
{
    /* save the current PC */
    if (mem_push(registers[PC].value) == -1) { return -1; }
    if (core_jmp() == -1) { return -1; }

    return 0;
}

/** 
 * Implements the RET command. 
 * What this function do is to just pop from stack into 
 * the register PC. This assumes that the return address from 
 * a subroutine was previously pushed on the stack.
 */
int core_ret(void)
{
    int32_t pc = 0;
    if (mem_pop(&pc) == -1) { return -1; }
    
    /* restore the last saved PC */
    registers[PC].value = pc;
    
    return 0;
}
