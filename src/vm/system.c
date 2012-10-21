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
    
    char dis_instr[128] = { '\0' };
    disassemble(instruction, dis_instr, 0); // 0 for don't print hex
    
    logmsg(LOG_INFO, "Instruction <%s> at PC = %d caused Interrupt %d", 
           dis_instr, registers[PC].value, number);

    if (number < 0 || number > INT_MAX) {
        number = INT_ZERO;
    }

    /* multiply the interrupt number by 4 to get the address of the
     * interrupt pointer, which should store the address of a subroutine
     * (e.g. interrupt handler) */

    int32_t index = number * 4;
    uint8_t mem[4]  = { 0x0 };

    /* before we call the memory module, we disable interupts
     * (that is the effect of calling this function)
     * because the memory module calls interrupt() if for instance
     * the memory is not initialized.
     */
    set_register_bit(STAT, STAT_INTDISABLE);

    if (mem_read(mem, index, 4) == -1) { return -1; }

    /* reenable interrupt processing */
    unset_register_bit(STAT, STAT_INTDISABLE);

    int32_t address = mem_to_int(mem, 4);
    logmsg(LOG_INFO, "Interrupt: interrupt address is %d", address);

    /* if the memory location is we read in mem is just zeros,
     * call the default interrupt -- stop machine */
    if (address == 0) {
        running = 0;
    } else {
        if (mem_push(registers[PC].value) == -1) { return -1; }
        registers[PC].value = address;
        registers[PC].value -= 4;
    }
    
    return 0;
}

int core_int(void)
{
    int8_t b = instruction[1]; // last byte of the instruction
    /* reconstruct the 3 bytes interrupt number, 
     * which is stored in big endian order inside the instruction,
     * on this little endian intel machine */
    int ino = (b << 16) | (instruction[2] << 8) | (instruction[3]);
    return (interrupt(ino));
}


