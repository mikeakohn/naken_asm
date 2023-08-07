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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "common/var.h"

#define VAR_RETURN_VALUE(var,ret_type) \
  if (var->type == VAR_INT) \
  { \
    return (ret_type)var->value_int; \
  } \
    else \
  if (var->type == VAR_FLOAT) \
  { \
    return (ret_type)var->value_float; \
  }

void var_set_int(Var *var, int64_t value)
{
  var->value_int = value;
  var->type = VAR_INT;
}

void var_set_float(Var *var, double value)
{
  var->value_float = value;
  var->type = VAR_FLOAT;
}

uint32_t var_get_bin32(Var *var)
{
  uint32_t *data;
  float data_f;

  if (var->type == VAR_FLOAT)
  {
    data_f = (float)var->value_float;
    data = (uint32_t *)(void *)&data_f;

    return *data;
  }
    else
  {
    return (uint32_t)(var->value_int & 0xffffffff);
  }
}

uint64_t var_get_bin64(Var *var)
{
  uint64_t *data;
  double data_d;

  if (var->type == VAR_FLOAT)
  {
    data_d = (double)var->value_float;
    data = (uint64_t *)(void *)&data_d;
    return *data;
  }
    else
  {
    return var->value_int;
  }
}

int32_t var_get_int32(Var *var)
{
  VAR_RETURN_VALUE(var,int32_t);

  assert(0);

  return 0;
}

int64_t var_get_int64(Var *var)
{
  VAR_RETURN_VALUE(var,int64_t);

  assert(0);

  return 0;
}

float var_get_float(Var *var)
{
  VAR_RETURN_VALUE(var,float);

  assert(0);

  return 0;
}

double var_get_double(Var *var)
{
  VAR_RETURN_VALUE(var,double);

  assert(0);

  return 0;
}

int var_get_type(Var *var)
{
  return var->type;
}

int var_add(Var *var_d, Var *var_s)
{
  if (var_d->type == VAR_INT && var_s->type == VAR_INT)
  {
    var_d->value_int = var_d->value_int + var_s->value_int;
  }
    else
  {
    var_d->value_float = var_get_double(var_d) + var_get_double(var_s);
    var_d->type = VAR_FLOAT;
  }

  return 0;
}

int var_sub(Var *var_d, Var *var_s)
{
  if (var_d->type == VAR_INT && var_s->type == VAR_INT)
  {
    var_d->value_int = var_d->value_int - var_s->value_int;
  }
    else
  {
    var_d->value_float = var_get_double(var_d) - var_get_double(var_s);
    var_d->type = VAR_FLOAT;
  }

  return 0;
}

int var_mul(Var *var_d, Var *var_s)
{
  if (var_d->type == VAR_INT && var_s->type == VAR_INT)
  {
    var_d->value_int = var_d->value_int * var_s->value_int;
  }
    else
  {
    var_d->value_float = var_get_double(var_d) * var_get_double(var_s);
    var_d->type = VAR_FLOAT;
  }

  return 0;
}

int var_div(Var *var_d, Var *var_s)
{
  if (var_d->type == VAR_INT && var_s->type == VAR_INT)
  {
    var_d->value_int = var_d->value_int / var_s->value_int;
  }
    else
  {
    var_d->value_float = var_get_double(var_d) / var_get_double(var_s);
    var_d->type = VAR_FLOAT;
  }

  return 0;
}

int var_mod(Var *var_d, Var *var_s)
{
  if (var_d->type == VAR_FLOAT)
  {
    var_d->value_int = (int64_t)var_d->value_float;
    var_d->type = VAR_INT;
  }

  if (var_s->type == VAR_FLOAT)
  {
    var_s->value_int = (int64_t)var_s->value_float;
    var_s->type = VAR_INT;
  }

  var_d->value_int = var_d->value_int % var_s->value_int;

  return 0;
}

int var_shift_left(Var *var_d, Var *var_s)
{
  if (var_d->type == VAR_FLOAT)
  {
    //var_d->value_int = (int64_t)var_d->value_float;
    var_d->value_int = var_get_bin32(var_d);
    var_d->type = VAR_INT;
  }

  if (var_s->type == VAR_FLOAT)
  {
    var_s->value_int = (int64_t)var_s->value_float;
    var_s->type = VAR_INT;
  }

  var_d->value_int = var_d->value_int << var_s->value_int;

  return 0;
}

int var_shift_right(Var *var_d, Var *var_s)
{
  if (var_d->type == VAR_FLOAT)
  {
    //var_d->value_int = (int64_t)var_d->value_float;
    var_d->value_int = var_get_bin32(var_d);
    var_d->type = VAR_INT;
  }

  if (var_s->type == VAR_FLOAT)
  {
    var_s->value_int = (int64_t)var_s->value_float;
    var_s->type = VAR_INT;
  }

  var_d->value_int = var_d->value_int >> var_s->value_int;

  return 0;
}

int var_shift_right_unsigned(Var *var_d, Var *var_s)
{
  if (var_d->type == VAR_FLOAT)
  {
    //var_d->value_int = (int64_t)var_d->value_float;
    var_d->value_int = var_get_bin32(var_d);
    var_d->type = VAR_INT;
  }

  if (var_s->type == VAR_FLOAT)
  {
    var_s->value_int = (int64_t)var_s->value_float;
    var_s->type = VAR_INT;
  }

  var_d->value_int = (uint64_t)var_d->value_int >> var_s->value_int;

  return 0;
}

int var_and(Var *var_d, Var *var_s)
{
  if (var_d->type == VAR_FLOAT)
  {
    //var_d->value_int = (int64_t)var_d->value_float;
    var_d->value_int = var_get_bin32(var_d);
    var_d->type = VAR_INT;
  }

  if (var_s->type == VAR_FLOAT)
  {
    //var_s->value_int = (int64_t)var_s->value_float;
    var_s->value_int = var_get_bin32(var_s);
    var_s->type = VAR_INT;
  }

  var_d->value_int = var_d->value_int & var_s->value_int;

  return 0;
}

int var_or(Var *var_d, Var *var_s)
{
  if (var_d->type == VAR_FLOAT)
  {
    //var_d->value_int = (int64_t)var_d->value_float;
    var_d->value_int = var_get_bin32(var_d);
    var_d->type = VAR_INT;
  }

  if (var_s->type == VAR_FLOAT)
  {
    //var_s->value_int = (int64_t)var_s->value_float;
    var_s->value_int = var_get_bin32(var_s);
    var_s->type = VAR_INT;
  }

  var_d->value_int = var_d->value_int | var_s->value_int;

  return 0;
}

int var_xor(Var *var_d, Var *var_s)
{
  if (var_d->type == VAR_FLOAT)
  {
    //var_d->value_int = (int64_t)var_d->value_float;
    var_d->value_int = var_get_bin32(var_d);
    var_d->type = VAR_INT;
  }

  if (var_s->type == VAR_FLOAT)
  {
    //var_s->value_int = (int64_t)var_s->value_float;
    var_s->value_int = var_get_bin32(var_s);
    var_s->type = VAR_INT;
  }

  var_d->value_int = var_d->value_int ^ var_s->value_int;

  return 0;
}

int var_not(Var *var)
{
  if (var->type == VAR_FLOAT)
  {
    //var->value_int = (int64_t)var->value_float;
    var->value_int = var_get_bin32(var);
    var->type = VAR_INT;
  }

  var->value_int = ~var->value_int;

  return 0;
}


