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
#include "asm_avr8.h"
#include "assembler.h"
#include "table_avr8.h"
#include "get_tokens.h"
#include "eval_expression.h"

enum
{
  OPERAND_NONE,
  OPERAND_NUMBER,
  OPERAND_REG,
  OPERAND_X,
  OPERAND_Y,
  OPERAND_Z,
  OPERAND_X_PLUS,
  OPERAND_Y_PLUS,
  OPERAND_Z_PLUS,
  OPERAND_MINUS_X,
  OPERAND_MINUS_Y,
  OPERAND_MINUS_Z,
};

struct _operands
{
  int value;
  char type;
};

int parse_instruction_avr8(struct _asm_context *asm_context, char *instr)
{
char token[TOKENLEN];
int token_type;
char instr_case[TOKENLEN];
int n;

  lower_copy(instr_case, instr);

  //token_type=get_token(asm_context, token, TOKENLEN);
  //pushback(asm_context, token, token_type);

  n=0;
  while(table_avr8[n].instr!=NULL)
  {
    if (strcmp(table_avr8[n].instr, instr_case)==0)
    {
      switch(table_avr8[n].type)
      {
        case OP_NONE:
        case OP_BRANCH_S_K:
        case OP_BRANCH_K:
        default:
          break;
      }
    }
    n++;
  }

  print_error_unknown_instr(instr, asm_context);

  return -1;
}



