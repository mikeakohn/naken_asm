/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2019 by Michael Kohn
 *
 */

#ifndef NAKEN_ASM_EVAL_EXPRESSION_EX_H
#define NAKEN_ASM_EVAL_EXPRESSION_EX_H

#include "common/assembler.h"
#include "common/eval_expression.h"
#include "common/var.h"

#if 0
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
  OPER_LEFT_SHIFT,
  OPER_RIGHT_SHIFT,
  OPER_AND,
  OPER_XOR,
  OPER_OR
};
#endif

int eval_expression_ex(struct _asm_context *asm_context, struct _var *var);

#endif

