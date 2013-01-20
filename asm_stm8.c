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
#include "asm_stm8.h"
#include "assembler.h"
#include "disasm_stm8.h"
#include "get_tokens.h"
#include "eval_expression.h"

int parse_instruction_stm8(struct _asm_context *asm_context, char *instr)
{
//struct _operand operands[3];
int operand_count=0;
char token[TOKENLEN];
int token_type;
char instr_case[TOKENLEN];
//int paren_flag;
int num,n,r;
//int opcode;

  lower_copy(instr_case, instr);

  while(1)
  {
    token_type=get_token(asm_context, token, TOKENLEN);
    if (token_type==TOKEN_EOL) { break; }

    if (operand_count==0 && IS_TOKEN(token,'.'))
    {
      strcat(instr_case, ".");
      strcat(instr, ".");
      token_type=get_token(asm_context, token, TOKENLEN);
      strcat(instr, token);
      n=0;
      while(token[n]!=0) { token[n]=tolower(token[n]); n++; }
      strcat(instr_case, token);
      continue;
    }

  }

  if (asm_context->pass==1)
  {
    add_bin32(asm_context, 0, IS_OPCODE);
    return 4;
  }


  printf("Error: Unknown instruction '%s'  at %s:%d\n", instr, asm_context->filename, asm_context->line);

  return -1;
}


