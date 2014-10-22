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
#include "asm_stm8.h"
#include "assembler.h"
#include "disasm_stm8.h"
#include "tokens.h"
#include "eval_expression.h"
#include "table_stm8.h"

enum
{
  OPERAND_REG_A,
  OPERAND_REG_X,
  OPERAND_REG_Y,
  OPERAND_NUM8,
  OPERAND_ADDRESS8,
  OPERAND_ADDRESS16,
  OPERAND_ADDRESS24,
  OPERAND_INDEX_X,
  OPERAND_INDEX_Y,
  OPERAND_OFFSET8_INDEX_X,
  OPERAND_OFFSET16_INDEX_X,
  OPERAND_OFFSET8_INDEX_Y,
  OPERAND_OFFSET16_INDEX_Y,
  OPERAND_OFFSET8_INDEX_SP,
  OPERAND_INDIRECT8,
  OPERAND_INDIRECT16,
  OPERAND_INDIRECT8_X,
  OPERAND_INDIRECT16_X,
  OPERAND_INDIRECT8_Y,
};

int parse_instruction_stm8(struct _asm_context *asm_context, char *instr)
{
char instr_case[TOKENLEN];
char token[TOKENLEN];
int token_type;
int ret;
int n;

  lower_copy(instr_case, instr);

  // Find instruction
  n = 0;
  while(table_stm8[n].instr != NULL)
  {
    n++;
  }

  // Parse operands
  while(1)
  {
    token_type = tokens_get(asm_context, token, TOKENLEN);

    if (token_type == TOKEN_EOL || token_type == TOKEN_EOF)
    { 
      break;
    }
  }

  // Get opcodes
  n = 0;
  while(table_stm8[n].instr != NULL)
  {
    if (strcmp(table_stm8[n].instr,instr_case) == 0)
    {
    }
    n++;
  }
 
  token_type=tokens_get(asm_context, token, TOKENLEN);
  if (token_type!=TOKEN_EOL && token_type!=TOKEN_EOF)
  {
    print_error_unexp(token, asm_context);
    return -1;
  }

  return ret;
}

