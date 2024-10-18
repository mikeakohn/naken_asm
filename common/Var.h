/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2024 by Michael Kohn
 *
 */

#ifndef NAKEN_ASM_VAR_H
#define NAKEN_ASM_VAR_H

#include <stdint.h>
#include <assert.h>

enum
{
  VAR_INT,
  VAR_FLOAT,
};

class Var
{
public:
  Var() : type (VAR_INT), value_int (0)
  {
  }

  Var(Var &v)
  {
    type = v.type;
    value_int = v.value_int;
  }

  ~Var()
  {
  }

  template <typename TYPE>
  TYPE get_data()
  {
    switch (type)
    {
      case VAR_INT:   return (TYPE)value_int;
      case VAR_FLOAT: return (TYPE)value_float;
      default: break;
    }

    assert(false);
  }

  void clear()
  {
    value_int = 0;
    type = VAR_INT;
  }

  int32_t get_int32() { return get_data<int32_t>(); }
  int64_t get_int64() { return get_data<int64_t>(); }
  float get_float()   { return get_data<float>();    }
  double get_double() { return get_data<double>();   }

  int get_type() { return type; }

  void set_int(uint64_t value)
  {
    value_int = value;
    type = VAR_INT;
  }

  void set_int(const char *value)
  {
    value_int = atoll(value);
    type = VAR_INT;
  }

  void set_float(double value)
  {
    value_float = value;
    type = VAR_FLOAT;
  }

  void set_float(const char *value)
  {
    value_float = atof(value);
    type = VAR_FLOAT;
  }

  void negative()
  {
    switch (type)
    {
      case VAR_INT:   value_int   = -value_int;   break;
      case VAR_FLOAT: value_float = -value_float; break;
      default: assert(false);
    }
  }

  uint32_t get_bin32();
  uint64_t get_bin64();
  int mul(Var &var_d, Var &var_s);
  int div(Var &var_d, Var &var_s);
  int mod(Var &var_d, Var &var_s);
  int add(Var &var_d, Var &var_s);
  int sub(Var &var_d, Var &var_s);
  int logical_and(Var &var_d, Var &var_s);
  int logical_or(Var &var_d, Var &var_s);
  int logical_xor(Var &var_d, Var &var_s);
  int logical_not(Var &var_d);
  int shift_left(Var &var_d, Var &var_s);
  int shift_right_unsigned(Var &var_d, Var &var_s);
  int shift_right(Var &var_d, Var &var_s);

private:
  void to_int()
  {
    if (type == VAR_FLOAT)
    {
      //value_int = (int64_t)value_float;
      value_int = get_bin32();
      type = VAR_INT;
    }
  }

  int type;

  union
  {
    int64_t value_int;
    double value_float;
  };
};

#endif

