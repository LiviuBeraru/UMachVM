#ifndef REGISTERS_H
#define REGISTERS_H

#include <stdint.h> // for int32_t

#define REG_READ  1
#define REG_WRITE 2
#define REG_RW    3
#define NOREGS    64

#define PC   33
#define SP   34
#define FP   35
#define IR   36 // do we really need this?
#define STAT 37
#define ERR  38
#define HI   39
#define LO   40
#define CMPR 41
#define ZERO 0

typedef
struct core_register {
    int32_t value;
    int32_t mode;
    const char * const name;
}
Register;

extern Register registers[NOREGS];

Register* get_register(int regno);
Register* get_register_byname(const char *name);

int read_register   (int regno, int32_t* dest);
int read_registeru  (int regno, uint32_t* dest);
int write_register  (int regno, int32_t value);
int write_registeru (int regno, uint32_t value);
#endif
