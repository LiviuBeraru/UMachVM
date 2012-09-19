#ifndef ASM_FORMATS_H
#define ASM_FORMATS_H

#include <stdint.h> // uint8_t

int assembleNUL(char **items, int n, uint8_t instruction[4]);
int assembleNNN(char **items, int n, uint8_t instruction[4]);
int assembleR00(char **items, int n, uint8_t instruction[4]);
int assembleRNN(char **items, int n, uint8_t instruction[4]);
int assembleRR0(char **items, int n, uint8_t instruction[4]);
int assembleRRN(char **items, int n, uint8_t instruction[4]);
int assembleRRR(char **items, int n, uint8_t instruction[4]);

#endif
