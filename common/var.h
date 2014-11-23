/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2014 by Michael Kohn
 *
 */

#ifndef _VAR_H
#define _VAR_H

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
    uint64_t value_int;
    double value_float;
  };
};

void var_set_int(struct _var *var, uint64_t value);
void var_set_float(struct _var *var, double value);
uint32_t var_get_int32(struct _var *var);
uint64_t var_get_int64(struct _var *var);
float var_get_float(struct _var *var);
double var_get_double(struct _var *var);
int var_add(struct _var *var_d, struct _var *var_s);
int var_sub(struct _var *var_d, struct _var *var_s);
int var_mul(struct _var *var_d, struct _var *var_s);
int var_div(struct _var *var_d, struct _var *var_s);

#endif

