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

#ifndef NAKEN_ASM_EVAL_EXPRESSION_H
#define NAKEN_ASM_EVAL_EXPRESSION_H

#include "common/assembler.h"
#include "common/eval_expression.h"
#include "common/Operator.h"
#include "common/Var.h"

class EvalExpression
{
public:
  static int run(
    AsmContext *asm_context,
    Var &var,
    Operator &last_operator);

private:
  EvalExpression()  { }
  ~EvalExpression() { }

  class VarStack
  {
  public:

    VarStack() : ptr { 0 }
    {
    }

    int push(Var &var)
    {
      if (ptr >= 3) { return -1; }
      stack[ptr++] = var;
    }

    int size() { return ptr; }

    int pop_first()
    {
      assert(ptr > 0);

      for (int i = 0; i < ptr - 1; i++)
      {
        stack[i] = stack[i + 1];
      }

      ptr--;
    }

    int pop()
    {
      assert(ptr > 0);

      ptr--;
    }

    Var &get_first()
    {
      assert(ptr >= 1);

      return stack[0];
    }

    Var &get_second()
    {
      assert(ptr >= 2);

      return stack[ptr - 2];
    }

    Var &get_bottom()
    {
      assert(ptr >= 3);

      return stack[ptr - 1];
    }

    Var stack[3];
    int ptr;
  };

  static int operate(Var &var_d, Var &var_s, Operator &oper);
  static int parse_unary(AsmContext *asm_context, int64_t *num, int operation);
  static int get_quoted_literal(AsmContext *asm_context, char *token, int length);

};

int eval_expression(AsmContext *asm_context, Var &var);
int eval_expression(AsmContext *asm_context, int *num);

#endif

