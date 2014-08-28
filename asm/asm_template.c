/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2014 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "asm_common.h"
#include "asm_NAME.h"
#include "assembler.h"
#include "table_NAME.h"
#include "tokens.h"
#include "eval_expression.h"

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



