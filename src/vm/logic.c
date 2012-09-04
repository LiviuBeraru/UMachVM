#include <stdio.h>
#include <unistd.h>
#include "core.h"
#include "registers.h"

int core_and(void)
{
    int dest_no = instruction[1];
    int r1_no = instruction[2];
    int r2_no = instruction[3];
    
    int32_t r1_value = 0;
    int32_t r2_value = 0;
    int32_t dest_value = 0;
    
    if (read_register(r1_no, &r1_value) == -1) { return -1; }
    if (read_register(r2_no, &r2_value) == -1) { return -1; }
    
    dest_value = r1_value & r2_value;
    
    if (write_register(dest_no, dest_value) == -1) { return -1; }
    
    return 0;
}

int core_andi(void)
{
    int    dest_no   = instruction[1];
    int    r1_no     = instruction[2];
    /* if we declare the immediate byte as signed, 
     * then casting it to int fills the int with the sign bit, so 
     * if the byte is say -1 (0xFF), then the int will be filled 
     * with ones on the left side.
     * But what we want is to fill the int with zeros on the left. 
     * This can be achieved by declaring the byte unsigned.
     */
    uint8_t immediate = instruction[3];
    
    int32_t r1_value   = 0;
    int32_t dest_value = 0;
    
    if (read_register(r1_no, &r1_value) == -1) { return -1; }
    
    dest_value = r1_value & immediate;
    
    if (write_register(dest_no, dest_value) == -1) { return -1; }
    
    return 0;
}

int core_or(void)
{ // copy paste from and
    int dest_no = instruction[1];
    int r1_no   = instruction[2];
    int r2_no   = instruction[3];
    
    int32_t r1_value   = 0;
    int32_t r2_value   = 0;
    int32_t dest_value = 0;
    
    if (read_register(r1_no, &r1_value) == -1) { return -1; }
    if (read_register(r2_no, &r2_value) == -1) { return -1; }
    
    dest_value = r1_value | r2_value;
    
    if (write_register(dest_no, dest_value) == -1) { return -1; }
    
    return 0;
}

int core_ori(void)
{// copy paste from andi
    int    dest_no    = instruction[1];
    int    r1_no      = instruction[2];
    uint8_t immediate = instruction[3];
    
    int32_t r1_value   = 0;
    int32_t dest_value = 0;
    
    if (read_register(r1_no, &r1_value) == -1) { return -1; }
    
    dest_value = r1_value | immediate;
    
    if (write_register(dest_no, dest_value) == -1) { return -1; }
    
    return 0;
}

int core_xor(void)
{
    int dest_no = instruction[1];
    int r1_no   = instruction[2];
    int r2_no   = instruction[3];
    
    int32_t r1_value   = 0;
    int32_t r2_value   = 0;
    int32_t dest_value = 0;
    
    if (read_register(r1_no, &r1_value) == -1) { return -1; }
    if (read_register(r2_no, &r2_value) == -1) { return -1; }
    
    dest_value = r1_value ^ r2_value;
    
    if (write_register(dest_no, dest_value) == -1) { return -1; }
    
    return 0;
}

int core_xori(void)
{// copy paste from andi
    int    dest_no   = instruction[1];
    int    r1_no     = instruction[2];
    uint8_t immediate = instruction[3];
    
    int32_t r1_value   = 0;
    int32_t dest_value = 0;
    
    if (read_register(r1_no, &r1_value) == -1) { return -1; }
    
    dest_value = r1_value ^ immediate;
    
    if (write_register(dest_no, dest_value) == -1) { return -1; }
    
    return 0;
}

int core_not(void)
{
    int dest_no = instruction[1];
    int reg_no  = instruction[2];
    
    int32_t value = 0;
    if (read_register(reg_no, &value) == -1) { return -1; }
    value = ~value;
    
    if (write_register(dest_no, value) == -1) { return -1; }
    
    return 0;
}

int core_noti(void)
{
    int      dest_no = instruction[1];
    uint16_t imm16 = (instruction[2] << 8) | instruction[3];
    int32_t  value = imm16; // fills with zero on the left
    value = ~value; // invert bits
    
    if (write_register(dest_no, value) == -1) { return -1; }
    return 0;
}

int core_nand(void)
{
    int dest_no = instruction[1];
    int reg1_no = instruction[2];
    int reg2_no = instruction[3];
    
    int32_t dest_value = 0;
    int32_t reg1_value = 0;
    int32_t reg2_value = 0;
    
    if (read_register(reg1_no, &reg1_value) == -1) { return -1; }
    if (read_register(reg2_no, &reg2_value) == -1) { return -1; }
    
    dest_value = ~(reg1_value & reg2_value); // nand = not(and)
    
    if (write_register(dest_no, dest_value) == -1) { return -1; }
    
    return 0;
}

int core_nandi(void)
{
    int    dest_no = instruction[1];
    int    reg_no  = instruction[2];
    uint8_t imm8   = instruction[3];
    
    int32_t dest_value = 0;
    int32_t reg1_value = 0;
    int32_t immediate  = imm8;
    
    if (read_register(reg_no, &reg1_value) == -1) { return -1; }
    
    dest_value = ~(reg1_value & immediate);
    
    if (write_register(dest_no, dest_value) == -1) { return -1; }
    
    return 0;
}

int core_nor(void)
{
    /* extract register numbers from the instruction */
    int dest_no = instruction[1];
    int reg1_no = instruction[2];
    int reg2_no = instruction[3];
    
    int32_t dest_value = 0; // value of destination register
    int32_t reg1_value = 0; // value of first operand
    int32_t reg2_value = 0; // value of second operand
    
    if (read_register(reg1_no, &reg1_value) == -1) { return -1; }
    if (read_register(reg2_no, &reg2_value) == -1) { return -1; }
    
    dest_value = ~(reg1_value | reg2_value); // nor = not(or)
    
    if (write_register(dest_no, dest_value) == -1) { return -1; }
    
    return 0;
}

int core_nori(void)
{
    int    dest_no = instruction[1];
    int    reg_no  = instruction[2];
    uint8_t imm8   = instruction[3];
    
    int32_t dest_value = 0;
    int32_t reg1_value = 0;
    int32_t immediate  = imm8;
    
    if (read_register(reg_no, &reg1_value) == -1) { return -1; }
    
    dest_value = ~(reg1_value | immediate); // A nor B = not (A or B)
    
    if (write_register(dest_no, dest_value) == -1) { return -1; }
    
    return 0;
}

int core_shl(void)
{
    /* extract register numbers from the instruction */
    uint8_t dest_no = instruction[1];
    uint8_t reg1_no = instruction[2];
    uint8_t reg2_no = instruction[3];
    
    /* register value to read and write */
    int32_t dest_value = 0;
    int32_t reg1_value = 0;
    int32_t reg2_value = 0;
    
    /* read register values */
    if (read_register(reg1_no, &reg1_value) == -1) { return -1; }
    if (read_register(reg2_no, &reg2_value) == -1) { return -1; }
    
    dest_value = reg1_value << reg2_value;
    
    /* write back the result */
    if (write_register(dest_no, dest_value) == -1) { return -1; }
    
    return 0;
}

int core_shli(void)
{
    int    dest_no = instruction[1];
    int    reg_no  = instruction[2];
    uint8_t imm8   = instruction[3];
    
    int32_t dest_value = 0;
    int32_t reg1_value = 0;
    int32_t immediate  = imm8;
    
    if (read_register(reg_no, &reg1_value) == -1) { return -1; }
    
    dest_value = reg1_value << immediate;
    
    if (write_register(dest_no, dest_value) == -1) { return -1; }
    
    return 0;
}

int core_shr(void)
{// copy paste from shl
   /* extract register numbers from the instruction */
    uint8_t dest_no = instruction[1];
    uint8_t reg1_no = instruction[2];
    uint8_t reg2_no = instruction[3];
    
    /* register value to read and write */
    /* unsigned because right shifting unsigned values discards the sign bit */
    uint32_t dest_value = 0;
    uint32_t reg1_value = 0;
    uint32_t reg2_value = 0;
    
    /* read register values */
    if (read_registeru(reg1_no, &reg1_value) == -1) { return -1; }
    if (read_registeru(reg2_no, &reg2_value) == -1) { return -1; }
    
    dest_value = reg1_value >> reg2_value;
    
    /* write back the result */
    if (write_registeru(dest_no, dest_value) == -1) { return -1; }
    
    return 0;
}

int core_shri(void)
{
    uint8_t dest_no = instruction[1];
    uint8_t reg_no  = instruction[2];
    uint8_t imm8    = instruction[3];
    
    /* work with unsigned values to discard the sign bit when shifting */
    uint32_t dest_value = 0;
    uint32_t reg1_value = 0;
    uint32_t immediate  = imm8;
    
    if (read_registeru(reg_no, &reg1_value) == -1) { return -1; }
    
    dest_value = reg1_value >> immediate;
    
    if (write_registeru(dest_no, dest_value) == -1) { return -1; }
    
    return 0;
}

int core_shra(void)
{
    /* extract register numbers from the instruction */
    uint8_t dest_no = instruction[1];
    uint8_t reg1_no = instruction[2];
    uint8_t reg2_no = instruction[3];
    
    /* register value to read and write */
    /* signed because right shifting signed values preserves the sign bit */
    int32_t dest_value = 0;
    int32_t reg1_value = 0;
    int32_t reg2_value = 0;
    
    /* read register values */
    if (read_register(reg1_no, &reg1_value) == -1) { return -1; }
    if (read_register(reg2_no, &reg2_value) == -1) { return -1; }
    
    dest_value = reg1_value >> reg2_value;
    
    /* write back the result */
    if (write_registeru(dest_no, dest_value) == -1) { return -1; }
    
    return 0;
}

int core_shrai(void)
{
    uint8_t dest_no = instruction[1];
    uint8_t reg_no  = instruction[2];
    uint8_t imm8   = instruction[3];
    
    /* work with signed values to preserve the sign bit when shifting */
    int32_t dest_value = 0;
    int32_t reg1_value = 0;
    int32_t immediate  = imm8;
    
    if (read_register(reg_no, &reg1_value) == -1) { return -1; }
    
    dest_value = reg1_value >> immediate;
    
    if (write_register(dest_no, dest_value) == -1) { return -1; }
    
    return 0;
}

int core_rotl(void)
{
    uint8_t dest_no = instruction[1];
    uint8_t reg1_no = instruction[2];
    uint8_t reg2_no = instruction[3];
    
    uint32_t reg1_value = 0;
    uint32_t reg2_value = 0;
    uint32_t dest_value = 0;
    
    /* read register values */
    if (read_registeru(reg1_no, &reg1_value) == -1) { return -1; }
    if (read_registeru(reg2_no, &reg2_value) == -1) { return -1; }
    
    reg2_value = reg2_value % 32;
    
    /* see Hacker's Delight, by Henry S. Warren, Jr., section 2-14 */
    dest_value = (reg1_value << reg2_value) | (reg1_value >> (32 - reg2_value));
        
    if (write_registeru(dest_no, dest_value) == -1) { return -1; }
    
    return 0;
}

int core_rotli(void)
{
    uint8_t dest_no = instruction[1];
    uint8_t reg_no = instruction[2];
    uint8_t imm8    = instruction[3];
    
    uint32_t reg_value = 0;
    uint32_t immediate  = imm8;
    uint32_t dest_value = 0;
    
    /* read register values */
    if (read_registeru(reg_no, &reg_value) == -1) { return -1; }
    
    immediate = immediate % 32;
    
    /* see Hacker's Delight, by Henry S. Warren, Jr., section 2-14 */
    dest_value = (reg_value << immediate) | (reg_value >> (32 - immediate));
        
    if (write_registeru(dest_no, dest_value) == -1) { return -1; }
    
    return 0;
}

int core_rotr(void)
{
    uint8_t dest_no = instruction[1];
    uint8_t reg1_no = instruction[2];
    uint8_t reg2_no = instruction[3];
    
    uint32_t reg1_value = 0;
    uint32_t reg2_value = 0;
    uint32_t dest_value = 0;
    
    /* read register values */
    if (read_registeru(reg1_no, &reg1_value) == -1) { return -1; }
    if (read_registeru(reg2_no, &reg2_value) == -1) { return -1; }
    
    reg2_value = reg2_value % 32;
    
    /* see Hacker's Delight, by Henry S. Warren Jr., section 2-14
     * (Rotate Shifts) */
    dest_value = (reg1_value >> reg2_value) | (reg1_value << (32 - reg2_value));
        
    if (write_registeru(dest_no, dest_value) == -1) { return -1; }
    
    return 0;
}

int core_rotri(void)
{
    uint8_t dest_no = instruction[1];
    uint8_t reg_no = instruction[2];
    uint8_t imm8    = instruction[3];
    
    uint32_t reg_value = 0;
    uint32_t immediate  = imm8;
    uint32_t dest_value = 0;
    
    /* read register values */
    if (read_registeru(reg_no, &reg_value) == -1) { return -1; }
    
    immediate = immediate % 32;
    
    /* see Hacker's Delight, by Henry S. Warren, Jr., section 2-14 */
    dest_value = (reg_value >> immediate) | (reg_value << (32 - immediate));
        
    if (write_registeru(dest_no, dest_value) == -1) { return -1; }
    
    return 0;
}


