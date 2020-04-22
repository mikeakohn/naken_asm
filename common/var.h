/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2020 by Michael Kohn
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

struct _var
{
  int type;
  union
  {
    int64_t value_int;
    double value_float;
  };
};

void var_set_int(struct _var *var, int64_t value);
void var_set_float(struct _var *var, double value);
uint32_t var_get_bin32(struct _var *var);
uint64_t var_get_bin64(struct _var *var);
int32_t var_get_int32(struct _var *var);
int64_t var_get_int64(struct _var *var);
float var_get_float(struct _var *var);
double var_get_double(struct _var *var);
int var_get_type(struct _var *var);
int var_add(struct _var *var_d, struct _var *var_s);
int var_sub(struct _var *var_d, struct _var *var_s);
int var_mul(struct _var *var_d, struct _var *var_s);
int var_div(struct _var *var_d, struct _var *var_s);
int var_mod(struct _var *var_d, struct _var *var_s);
int var_shift_left(struct _var *var_d, struct _var *var_s);
int var_shift_left_unsigned(struct _var *var_d, struct _var *var_s);
int var_shift_right(struct _var *var_d, struct _var *var_s);
int var_and(struct _var *var_d, struct _var *var_s);
int var_or(struct _var *var_d, struct _var *var_s);
int var_xor(struct _var *var_d, struct _var *var_s);
int var_not(struct _var *var_d);

#endif

