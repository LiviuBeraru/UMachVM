#ifndef UMACH_H
#define UMACH_H

struct umach_options {
    int    disassemble;
    int    debug;
    size_t memory;
    int    hexa;
    int    verbose;
};

/** Global options given when starting the maschine */
extern struct umach_options options;

#endif
