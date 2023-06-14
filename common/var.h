/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2023 by Michael Kohn
 *
 */

#ifndef NAKEN_ASM_VAR_H
#define NAKEN_ASM_VAR_H

#include <stdint.h>

enum
{
  VAR_INT,
  VAR_FLOAT,
};

typedef struct _var
{
  int type;
  union
  {
    int64_t value_int;
    double value_float;
  };
} Var;

void var_set_int(Var *var, int64_t value);
void var_set_float(Var *var, double value);
uint32_t var_get_bin32(Var *var);
uint64_t var_get_bin64(Var *var);
int32_t var_get_int32(Var *var);
int64_t var_get_int64(Var *var);
float var_get_float(Var *var);
double var_get_double(Var *var);
int var_get_type(Var *var);
int var_add(Var *var_d, Var *var_s);
int var_sub(Var *var_d, Var *var_s);
int var_mul(Var *var_d, Var *var_s);
int var_div(Var *var_d, Var *var_s);
int var_mod(Var *var_d, Var *var_s);
int var_shift_left(Var *var_d, Var *var_s);
int var_shift_left_unsigned(Var *var_d, Var *var_s);
int var_shift_right(Var *var_d, Var *var_s);
int var_and(Var *var_d, Var *var_s);
int var_or(Var *var_d, Var *var_s);
int var_xor(Var *var_d, Var *var_s);
int var_not(Var *var_d);

#endif

