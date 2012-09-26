/**
 * @file uasm.h
 * @brief Main file of the UMach assembler
 * 
 */

#ifndef UASM_H
#define UASM_H

/** 
 * Interrupt table size in bytes.
 * 
 * The interrupt table is the memory region just 
 * before the code segment of the UMach machine.
 * See also the UMach specification.
 */
#define ITABLE_SIZE 256

/**
 * Maximal size of an assembler label in ascii characters.
 */
#define LABEL_LENGHT 128

/** 
 * Print the current file name, line number and some message to stderr. 
 *
 * The printed file name and line number are related to the current 
 * file being read and assembled.
 *
 * @param format String format analog to the one you pass to printf.
 * @param ...    analog to the printf arguments.
 */
void printerror(const char* format, ...);

#endif 
