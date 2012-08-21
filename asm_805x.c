/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2012 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "asm_805x.h"
#include "asm_common.h"
#include "assembler.h"
#include "disasm_arm.h"
#include "get_tokens.h"
#include "eval_expression.h"

#if 0
static int get_register_805x(char *token)
{

  return -1;
}
#endif

int parse_instruction_805x(struct _asm_context *asm_context, char *instr)
{
char instr_lower_mem[TOKENLEN];
char *instr_lower=instr_lower_mem;
char token[TOKENLEN];
int token_type;

  lower_copy(instr_lower, instr);
  token_type=get_token(asm_context, token, TOKENLEN);

  printf("Error: Not implemented\n");

  return -1;
}


