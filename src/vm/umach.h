#ifndef UMACH_H
#define UMACH_H

struct umach_options {
    int    assemble;
    int    disassemble;
    int    debug;
    size_t memory;
    int    hexa;
    int    verbose;
};

extern struct umach_options options;

#endif
