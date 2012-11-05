#ifndef ASM_FORMATS_H
#define ASM_FORMATS_H

#include <stdint.h> // uint8_t

int assemble_nul(asm_context_t *cntxt, char *items[], int n, uint8_t instruction[4]);
int assemble_nnn(asm_context_t *cntxt, char *items[], int n, uint8_t instruction[4]);
int assemble_r00(asm_context_t *cntxt, char *items[], int n, uint8_t instruction[4]);
int assemble_rnn(asm_context_t *cntxt, char *items[], int n, uint8_t instruction[4]);
int assemble_rr0(asm_context_t *cntxt, char *items[], int n, uint8_t instruction[4]);
int assemble_rrn(asm_context_t *cntxt, char *items[], int n, uint8_t instruction[4]);
int assemble_rrr(asm_context_t *cntxt, char *items[], int n, uint8_t instruction[4]);

#endif
