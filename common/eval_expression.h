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

#ifndef NAKEN_ASM_EVAL_EXPRESSION_H
#define NAKEN_ASM_EVAL_EXPRESSION_H

#include "common/assembler.h"

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
  OPER_LEFT_SHIFT,
  OPER_RIGHT_SHIFT,
  OPER_PLUS,
  OPER_MINUS,
  OPER_AND,
  OPER_XOR,
  OPER_OR
};

int eval_expression(struct _asm_context *asm_context, int *num);

#endif

