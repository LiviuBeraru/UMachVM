#include "core.h"      // instruction[]
#include "logmsg.h"
#include "registers.h" // for value of PC
#include "disassemble.h" // disassemble current instruction 

int interrupt(int number)
{
    char dis_instr[128] = { '\0' };
    disassemble(instruction, dis_instr, 0); // 0 for don't print hex
    
    logmsg(LOG_INFO, "Instruction <%s> at PC = %d caused Interrupt %d", 
           dis_instr, registers[PC].value, number);
    running = 0;
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


