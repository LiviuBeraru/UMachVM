#include "core.h"      // State 
#include "regcheck.h"  // CHECK_READ, CHECK_WRITE

#include <stdio.h>

int core_add(void) 
{
    State *state = core_getstate();
    int d = state -> instruction[1];
    int a = state -> instruction[2];
    int b = state -> instruction[3];
    
    CHECK_WRITE(d);
    CHECK_READ(a);
    CHECK_READ(b);

    a = state->registers[a].value;
    b = state->registers[b].value;

    state->registers[d].value = a + b;

    return 0;
}

int core_addu(void) 
{
    State *state = core_getstate();;
    uint32_t d = state->instruction[1];
    uint32_t a = state->instruction[2];
    uint32_t b = state->instruction[3];
    
    CHECK_WRITE(d);
    CHECK_READ(a);
    CHECK_READ(b);
    
    a = state->registers[a].value;
    b = state->registers[b].value;
    
    state->registers[d].value = a + b;
    
    return 0;
}

int core_addi(void) 
{
    State *state = core_getstate();;
    int d = state->instruction[1];
    int r = state->instruction[2];
    int v = state->instruction[3];
    
    CHECK_WRITE(d);
    CHECK_READ(r);
    
    state->registers[d].value = state->registers[r].value + v;
    
    return 0;
}

int core_sub(void) 
{
    printf("%s\n", __func__);
    return 0;
}

int core_subu(void) 
{
    printf("%s\n", __func__);
    return 0;
}

int core_subi(void) 
{
    printf("%s\n", __func__);
    return 0;
}

int core_mul(void) 
{
    printf("%s\n", __func__);
    return 0;
}

int core_mulu(void) 
{
    printf("%s\n", __func__);
    return 0;
}

int core_muli(void) 
{
    printf("%s\n", __func__);
    return 0;
}

int core_div(void) 
{
    printf("%s\n", __func__);
    return 0;
}

int core_divu(void) 
{
    printf("%s\n", __func__);
    return 0;
}

int core_divi(void) 
{
    printf("%s\n", __func__);
    return 0;
}

int core_abs(void) 
{
    printf("%s\n", __func__);
    return 0;
}

int core_neg  (void)
{
    printf("%s\n", __func__);
    return 0;
}

int core_inc  (void)
{
    State *state = core_getstate();
    int r = state->instruction[1];
    
    CHECK_READ(r);
    CHECK_WRITE(r);
    
    state->registers[r].value++;
    
    return 0;
}

int core_dec  (void)
{
    State *state = core_getstate();
    int r = state->instruction[1];
    
    CHECK_READ(r);
    CHECK_WRITE(r);
    
    state->registers[r].value--;
    
    return 0;
}

int core_mod  (void)
{
    printf("%s\n", __func__);
    return 0;
}

int core_modi (void)
{
    printf("%s\n", __func__);
    return 0;
}
