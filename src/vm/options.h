#ifndef OPTIONS_H
#define OPTIONS_H

#include <stddef.h> // size_t

/**
 * Options given to this programm.
 */
struct umach_options {
    int    disassemble; /// disassemble file
    int    debug;       /// start in debug mode
    size_t memory;      /// how much memory in bytes
    int    hexa;        /// hexadecimal output in some functions
    int    verbose;     /// verbose level
};

/** Global options given when starting this program.
 * This object is defined in options.c.
 */
extern struct umach_options options;

#endif
