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

#include "common/Var.h"

uint32_t Var::get_bin32()
{
  uint32_t *data;
  float data_f;

  if (type == VAR_FLOAT)
  {
    data_f = (float)value_float;
    data = (uint32_t *)(void *)&data_f;

    return *data;
  }
    else
  {
    return (uint32_t)(value_int & 0xffffffff);
  }
}

uint64_t Var::get_bin64()
{
  uint64_t *data;
  double data_d;

  if (type == VAR_FLOAT)
  {
    data_d = (double)value_float;
    data = (uint64_t *)(void *)&data_d;
    return *data;
  }
    else
  {
    return value_int;
  }
}

int Var::mul(Var &var_d, Var &var_s)
{
  if (var_d.type == VAR_INT && var_s.type == VAR_INT)
  {
    value_int = var_d.value_int * var_s.value_int;
  }
    else
  {
    value_float = var_d.get_double() * var_s.get_double();
    type = VAR_FLOAT;
  }

  return 0;
}

int Var::div(Var &var_d, Var &var_s)
{
  if (var_d.type == VAR_INT && var_s.type == VAR_INT)
  {
    value_int = var_d.value_int / var_s.value_int;
  }
    else
  {
    value_float = var_d.get_double() / var_s.get_double();
    type = VAR_FLOAT;
  }

  return 0;
}

int Var::mod(Var &var_d, Var &var_s)
{
  var_d.to_int();
  var_s.to_int();

  value_int = var_d.value_int % var_s.value_int;

  return 0;
}

int Var::add(Var &var_d, Var &var_s)
{
  if (var_d.type == VAR_INT && var_s.type == VAR_INT)
  {
    value_int = var_d.value_int + var_s.value_int;
  }
    else
  {
    value_float = var_d.get_double() + var_s.get_double();
    type = VAR_FLOAT;
  }

  return 0;
}

int Var::sub(Var &var_d, Var &var_s)
{
  if (var_d.type == VAR_INT && var_s.type == VAR_INT)
  {
    value_int = var_d.value_int - var_s.value_int;
  }
    else
  {
    value_float = var_d.get_double() - var_s.get_double();
    type = VAR_FLOAT;
  }

  return 0;
}

int Var::logical_and(Var &var_d, Var &var_s)
{
  var_d.to_int();
  var_s.to_int();

  value_int = var_d.value_int & var_s.value_int;

  return 0;
}

int Var::logical_or(Var &var_d, Var &var_s)
{
  var_d.to_int();
  var_s.to_int();

  value_int = var_d.value_int | var_s.value_int;

  return 0;
}

int Var::logical_xor(Var &var_d, Var &var_s)
{
  var_d.to_int();
  var_s.to_int();

  value_int = var_d.value_int ^ var_s.value_int;

  return 0;
}

int Var::logical_not(Var &var_d)
{
  var_d.to_int();

  value_int = ~var_d.value_int;

  return 0;
}

int Var::shift_left(Var &var_d, Var &var_s)
{
  var_d.to_int();
  var_s.to_int();

  value_int = var_d.value_int << var_s.value_int;

  return 0;
}

int Var::shift_right(Var &var_d, Var &var_s)
{
  var_d.to_int();
  var_s.to_int();

  value_int = var_d.value_int >> var_s.value_int;

  return 0;
}

int Var::shift_right_unsigned(Var &var_d, Var &var_s)
{
  var_d.to_int();
  var_s.to_int();

  value_int = (uint64_t)var_d.value_int >> var_s.value_int;

  return 0;
}

