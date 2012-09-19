#ifndef REGISTERS_H
#define REGISTERS_H

#include <stdint.h> // uint8_t

/** maximal number of registers */
#define NOREGS    64

/* special register numbers */

#define PC   33   // program counter
#define DS   34   // data segment begin
#define HS   35   // heap segment begin
#define HE   36   // heap segmend end (last byte)
#define SP   37   // stack pointer
#define FP   38   // frame pointer
#define IR   39   // instruction register
#define STAT 40   // status register
#define ERR  41   // error register
#define HI   42   // higher 32 bits of mul/div
#define LO   43   // lower 32 bits of mul/div
#define CMPR 44   // comparison result register
#define ZERO 0    // zero register

/** Register structure. */
typedef
struct core_register {
    uint8_t regno;
    const char * const name;
}
Register;

extern Register registers[NOREGS];

Register* get_register_byname(const char *name);

#endif
