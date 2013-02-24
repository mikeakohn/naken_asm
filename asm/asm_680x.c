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

int parse_instruction_680x(struct _asm_context *asm_context, char *instr)
{
char token[TOKENLEN];
int token_type;
char instr_case[TOKENLEN];
int n;

  lower_copy(instr_case, instr);

/*
  n=0;
  while(tms_instr_table[n].instr!=NULL)
  {
    if (strcmp(instr_case, tms_instr_table[n].instr)==0 &&
        tms_instr_table[n].op1000!=0xffff)
    {
      add_bin8(asm_context, tms_instr_table[n].op1000, IS_OPCODE);
      return 1;
    }
    n++;
  }
*/

/*
  for (n=0; n<0xff; n++)
  {
    if (strcmp(instr_case, m680x_table[n])==0)
    {
      token_type=get_token(asm_context, token, TOKENLEN);
      if (token_type!=TOKEN_NUMBER)
      {
        if (asm_context->pass==1) { return 1; }
        print_error_unexp(token, asm_context);
        return -1;
      }
*/

/*
      int num=atoi(token);
      if (num<0 || num>3)
      {
        print_error_range("Constant", 0, 3, asm_context);
        return -1;
      }

      add_bin8(asm_context, ((0xc+n)<<2)|num, IS_OPCODE);
*/

/*
      return 1;
    }
  }
*/

  print_error_unknown_instr(instr, asm_context);

  return -1;
}



