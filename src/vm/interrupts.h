#ifndef INTERRUPTS_H
#define INTERRUPTS_H

/* Interrupt numbers */

/*  0 - 15 core interrupts */
// Operation fails
#define INT_ZERO           0    // default interrupt
#define INT_DIVNULL        1    // division by null
#define INT_AOVERFLOW      2    // arithmetic overflow
// arguments wrong
#define INT_INVALID_CMD    8    // invalid command (opcode)
#define INT_INVALID_REG    9    // invalid register number

/* 16 - 31 memory interrupts */
#define INT_INVALID_MEM    16   // invalid memory address
#define INT_FORBID_MEM     17   // forbidden memory address
#define INT_STACK_ERR      24   // stack error
#define INT_STACK_OVERFLOW 26   // stack overflow
#define INT_INSUFF_MEM     31   // insufficient memory

/* 32 - 47 IO Interrupts */
#define INT_NO_IOPORT      32   // no such I/O port

/* 48 - 63 system interrupts */
#define INT_INTERNAL_ERR   56   // internal error (broken machine)
#define INT_SYSCALL        63   // syscall for operating system


#endif
