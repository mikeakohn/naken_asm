/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2013 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "asm_common.h"
#include "asm_tms1000.h"
#include "assembler.h"
#include "disasm_tms1000.h"
#include "get_tokens.h"
#include "eval_expression.h"

int parse_instruction_tms1000(struct _asm_context *asm_context, char *instr)
{
char token[TOKENLEN];
int token_type;
char instr_case[TOKENLEN];

  lower_copy(instr_case, instr);

  //token_type=get_token(asm_context, token, TOKENLEN);

  print_error_unknown_instr(instr, asm_context);

  return -1;
}


