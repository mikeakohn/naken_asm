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

#include <stdint.h>
#include <inttypes.h>

#include "common/assembler.h"
#include "common/eval_expression.h"
#include "common/Operator.h"
#include "common/Var.h"

class EvalExpression
{
public:
  static int run(AsmContext *asm_context, Var &var, bool is_paren);

private:
  EvalExpression()  { }
  ~EvalExpression() { }

  class VarStack
  {
  public:

    VarStack() : ptr (0)
    {
    }

    int push(Var &var)
    {
      if (ptr >= 3) { return -1; }
      stack[ptr++] = var;

      return 0;
    }

    int push_front(Var &var)
    {
      if (ptr >= 3) { return -1; }

      for (int n = ptr; n > 0; n--)
      {
        stack[n] = stack[n - 1];
      }

      stack[0] = var;
      ptr++;

      return 0;
    }

    int push_int(uint64_t value)
    {
      Var var;
      var.set_int(value);
      return push(var);
    }

    int push_int(const char *token)
    {
      Var var;
      var.set_int(token);
      return push(var);
    }

    int push_float(const char *token)
    {
      Var var;
      var.set_float(token);
      return push(var);
    }

    int size()      { return ptr; }
    bool is_empty() { return ptr == 0; }

    Var pop_first()
    {
      assert(ptr > 0);

      Var first = stack[0];

      for (int i = 0; i < ptr - 1; i++)
      {
        stack[i] = stack[i + 1];
      }

      ptr--;

      return first;
    }

    Var pop()
    {
      assert(ptr > 0);
      return stack[--ptr];
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

    Var &get_last()
    {
      assert(ptr >= 1);
      return stack[ptr - 1];
    }

    void dump()
    {
      printf("-- var_stack %d --\n", ptr);
      for (int n = 0; n < ptr; n++)
      {
        printf("  %d) %" PRId64 "\n", n, stack[n].get_int64());
      }
    }

    Var stack[3];
    int ptr;
  };

  class OperStack
  {
  public:
    OperStack() : ptr (0)
    {
    }

    void push(Operator &oper)
    {
      assert(ptr < 2);
      stack[ptr++] = oper;
    }

    Operator pop()
    {
      assert(ptr > 0);
      return stack[--ptr];
    }

    Operator pop_first()
    {
      assert(ptr > 0);
      Operator value = stack[0];
      stack[0] = stack[1];
      ptr--;

      return value;
    }

    int get_precedence_index()
    {
      assert(ptr > 0);

      if (ptr == 1) { return 0; }

      if (stack[0].precedence > stack[1].precedence) { return 1; }
      return 0;
    }

    int size()      { return ptr; }
    bool is_empty() { return ptr == 0; }

    void dump()
    {
      printf("-- oper_stack %d --\n", ptr);
      for (int n = 0; n < ptr; n++)
      {
        printf("  %d) %s\n", n, stack[n].to_string());
      }
    }

  private:
    Operator stack[2];
    int ptr;
  };

  static bool need_symbol(int count)
  {
    return count == 1 || count == 3;
  }

  static bool need_number(int count)
  {
    return count == 0 || count == 2 || count == 4;
  }

  static int execute_stack(VarStack &var_stack, OperStack &oper_stack);
  static int parse_unary_new(AsmContext *asm_context, Var &answer);
  static int get_quoted_literal(AsmContext *asm_context, char *token, int length);

};

int eval_expression(AsmContext *asm_context, Var &var);
int eval_expression(AsmContext *asm_context, int *num);

#endif

