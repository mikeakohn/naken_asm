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

#ifndef NAKEN_ASM_OPERATOR_H
#define NAKEN_ASM_OPERATOR_H

#include "Var.h"

class Operator
{
public:
  Operator() :
    operation  (OPER_UNSET),
    precedence (PREC_UNSET)
  {
  }

  Operator(const Operator &o)
  {
    operation  = o.operation;
    precedence = o.precedence;
  }

  Operator & operator= (const Operator &s)
  {
    operation  = s.operation;
    precedence = s.precedence;

    return *this;
  }

  bool is_unset() { return operation == OPER_UNSET; }
  bool is_set()   { return operation != OPER_UNSET; }

  void reset()
  {
    operation  = OPER_UNSET;
    precedence = PREC_UNSET;
  }

  static bool is_math_operator(const char *token);
  bool set_operator(const char *token);
  int execute(Var &var_d, Var &var_s);
  const char *to_string();

  enum
  {
    PREC_NOT,
    PREC_MUL,
    PREC_ADD,
    PREC_SHIFT,
    PREC_AND,
    PREC_XOR,
    PREC_OR,
    PREC_UNSET
  };

  enum
  {
    OPER_UNSET,
    OPER_NOT,
    OPER_MUL,
    OPER_DIV,
    OPER_MOD,
    OPER_PLUS,
    OPER_MINUS,
    OPER_SHIFT_L,
    OPER_SHIFT_R,
    OPER_AND,
    OPER_XOR,
    OPER_OR
  };

  int operation;
  int precedence;
};

#endif

