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
#include <string.h>

#include "common/assembler.h"
#include "common/eval_expression.h"
#include "common/eval_expression_ex.h"
#include "common/tokens.h"

int eval_expression(AsmContext *asm_context, int *num)
{
  Var var;

  int ret = eval_expression_ex(asm_context, var);
  *num = var.get_int32();

  return ret;
}

