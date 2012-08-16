/**
 * @file disassemble.h
 * @brief Disassemble functions
 */
#ifndef DISASSEMBLE_H
#define DISASSEMBLE_H

#include <stdio.h>

/**
 * @brief Disassemble 4 byte instruction into string representation
 *
 * The documentation of this function is not ready.
 *
 * @param intruction the instruction to disassemble
 * @param destination string where to store the string representation
 * @param printhex if the string representation should also contain the 
 *                 hexcode in instruction
 *
 * Example usage:
 * @code
 *      disassemble(in, dest, 0);
 * @endcode
 */
void disassemble(const uint8_t instruction[4], char* destination, int printhex);

int disassemble_file(FILE *file);

#endif
