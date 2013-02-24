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
#include "asm_680x.h"
#include "assembler.h"
#include "table_680x.h"
#include "get_tokens.h"
#include "eval_expression.h"

extern struct _m680x_table m680x_table[];

enum
{
  OPERAND_NONE,
  OPERAND_NUMBER,
  OPERAND_ADDRESS,
  OPERAND_ADDRESS_COMMA_X,
};

int parse_instruction_680x(struct _asm_context *asm_context, char *instr)
{
char token[TOKENLEN];
int token_type;
char instr_case[TOKENLEN];
int operand_type;
int operand_value;
int n;

  lower_copy(instr_case, instr);

  do
  {
    token_type=get_token(asm_context, token, TOKENLEN);
    if (token_type==TOKEN_EOL || token_type==TOKEN_EOF)
    {
      operand_type=OPERAND_NONE;
      break;
    }

    if (token_type==TOKEN_POUND)
    {
      operand_type=OPERAND_NUMBER;
      if (eval_expression(asm_context, &operand_value)!=0)
      {
        if (asm_context->pass==2)
        {
          print_error_illegal_expression(instr, asm_context);
          return -1;
        }

        operand_value=0xffff;
      }
    }
      else
    {
      operand_type=OPERAND_ADDRESS;
      pushback(asm_context, token, token_type);
      if (eval_expression(asm_context, &operand_value)!=0)
      {
        if (asm_context->pass==2)
        {
          print_error_illegal_expression(instr, asm_context);
          return -1;
        }

        operand_value=0xffff;
      }

      token_type=get_token(asm_context, token, TOKENLEN);
      if (token_type==TOKEN_EOL || token_type==TOKEN_EOF) { break; }
      if (IS_NOT_TOKEN(token,','))
      {
        print_error_unexp(token, asm_context);
        return -1;
      }

      if (expect_token_s(asm_context,"x")!=0) { return -1; }
      operand_type=OPERAND_ADDRESS_COMMA_X;
    }
  } while(0);

  for (n=0; n<256; n++)
  {
    if (m680x_table[n].instr==NULL) { continue; }

    if (strcmp(instr_case, m680x_table[n].instr)==0)
    {
      if (m680x_table[n].operand_type==M6800_OP_NONE &&
          operand_type==OPERAND_NONE)
      {
        add_bin8(asm_context, n, IS_OPCODE);
        return 1;
      }
    }
  }

  print_error_unknown_instr(instr, asm_context);

  return -1;
}



