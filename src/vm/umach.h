#ifndef UMACH_H
#define UMACH_H

struct umach_options {
    int    verbose;
    int    debug;
    size_t memory;
    char   *filename;
    int    disassemble;
};

extern struct umach_options options;

#endif
