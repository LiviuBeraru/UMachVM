#include "core.h"        // instruction[]
#include "logmsg.h"
#include "registers.h"   // for value of PC
#include "disassemble.h" // disassemble current instruction
#include "interrupts.h"  // INT_MAX
#include "memory.h"      // for ITABLE_SIZE

int interrupt(int number)
{
    /* if interrupts are disabled, don't do anything */
    if (isset_register_bit(STAT, STAT_INTDISABLE)) {
        return 0;
    }

    // buffer where we disassemble the current instruction
    char dis_instr[128] = { '\0' };
    disassemble(instruction, dis_instr, 0); // 0 for don't print hex
    
    logmsg(LOG_INFO, "Interrupt: <%s> at PC = %d caused Interrupt %d",
           dis_instr, registers[PC].value, number);

    if (number < 0 || number > INT_MAX) {
        logmsg(LOG_INFO, "Interrupt: wrong interrupt number %d, changing to %d",
               number, INT_ZERO);
        number = INT_ZERO;
    }

    /* multiply the interrupt number by 4 to get the address of the
     * interrupt pointer, which should store the address of a subroutine
     * (e.g. interrupt handler) */

    int32_t index = number * 4; // memory index
    uint8_t mem[4]  = { 0x0 };  // here whe copy the memory content at index

    /* before we call the memory module, we disable interupts
     * (that is the effect of calling this function)
     * because the memory module calls interrupt() if for instance
     * the memory is not initialized. If we don't block further interrupts
     * and if the memory calls interrupt, we're stuck in the loop of doom.
     */
    set_register_bit(STAT, STAT_INTDISABLE);

    if (mem_read(mem, index, 4) == -1) {
        /* something went wrong when reading from memory.
         * before we exit, remember to release the interrupts */
        unset_register_bit(STAT, STAT_INTDISABLE);
        return -1;
    }

    /* reenable interrupt processing */
    unset_register_bit(STAT, STAT_INTDISABLE);

    int32_t address = mem_to_int(mem, 4);
    logmsg(LOG_INFO, "Interrupt: address of interrupt handler is %d", address);

    /* if the memory location is we read in mem is just zeros,
     * do the default interrupt action -- stop the machine */
    if (address == 0) {
        running = 0;
    } else {
        /* push the current PC before we jump away. this way,
         * when an interrupt handler (which is an asm function terminating
         * with RET) can properly return. We are doing here the same thing
         * as in CALL (core_call), where we first push the current PC before
         * changing it.
         */
        if (mem_push(registers[PC].value) == -1) { return -1; }
        registers[PC].value = address;
        registers[PC].value -= 4; // core_execute adds 4 to PC
    }

    return 0;
}

int core_int(void)
{
    /* extract the 3 bytes interrupt number from instruction */
    int32_t intno = mem_to_int(instruction+1, 3);
    return (interrupt(intno));
}


