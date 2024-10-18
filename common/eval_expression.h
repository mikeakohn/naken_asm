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

  static int operate(Var &var_d, Var &var_s, Operator &oper);
  static int parse_unary(AsmContext *asm_context, int64_t *num, int operation);

};

int eval_expression(AsmContext *asm_context, Var &var);
int eval_expression(AsmContext *asm_context, int *num);

#endif

