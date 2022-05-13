/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2022 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "asm/common.h"
#include "asm/NAME.h"
#include "common/assembler.h"
#include "common/tokens.h"
#include "common/eval_expression.h"
#include "table/NAME.h"

enum
{
  OPERAND_NONE,
};

int parse_instruction_NAME(struct _asm_context *asm_context, char *instr)
{
  char token[TOKENLEN];
  int token_type;
  char instr_case[TOKENLEN];
  int n;

  lower_copy(instr_case, instr);

  //token_type=tokens_get(asm_context, token, TOKENLEN);
  //tokens_push(asm_context, token, token_type);

  print_error_unknown_instr(instr, asm_context);

  return -1;
}

