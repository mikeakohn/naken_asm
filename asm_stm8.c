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

static int parse_stm8_type1(struct _asm_context *asm_context, char *instr, int opcode_nibble)
{
char token[TOKENLEN];
int token_type;
int num;

  token_type=get_token(asm_context, token, TOKENLEN);

  if (token_type==TOKEN_POUND)
  {
    if (asm_context->pass==1)
    {
      eat_operand(asm_context);
    }

    if (eval_expression(asm_context, &num)!=0)
    {
      print_error_illegal_operands(instr, asm_context);
    }

    add_bin8(asm_context, 0xa0|opcode_nibble, IS_OPCODE);
    add_bin8(asm_context, num, IS_OPCODE);
    return 2;
  }

  return -1;
}

int parse_instruction_stm8(struct _asm_context *asm_context, char *instr)
{
char token[TOKENLEN];
int token_type;
char instr_case[TOKENLEN];
int n;

  lower_copy(instr_case, instr);

  n=0;
  while(stm8_single[n].instr!=NULL)
  {
    if (strcmp(stm8_single[n].instr, instr_case)==0)
    {
      add_bin8(asm_context, stm8_single[n].opcode, IS_OPCODE);
      return 1;
    }

    n++;
  }

  n=0;
  while(stm8_x_y[n].instr!=NULL)
  {
    if (strcmp(stm8_x_y[n].instr, instr_case)==0)
    {
      token_type=get_token(asm_context, token, TOKENLEN);

      if (strcasecmp("y", token)==0)
      {
        add_bin8(asm_context, 0x90, IS_OPCODE);
      }
        else
      if (strcasecmp("x", token)!=0)
      {
        print_error_unexp(token, asm_context);
        return -1;
      }

      add_bin8(asm_context, stm8_x_y[n].opcode, IS_OPCODE);
      return 1;
    }

    n++;
  }

  for (n=0; n<16; n++)
  {
    if (stm8_type1[n]==NULL) { continue; }
    if (strcmp(stm8_type1[n], instr_case)==0)
    {
      token_type=get_token(asm_context, token, TOKENLEN);

      if (strcasecmp(token, "a")==0)
      {
        token_type=get_token(asm_context, token, TOKENLEN);
        if (IS_NOT_TOKEN(token,','))
        {
          print_error_unexp(token, asm_context);
          return -1;
        }

        return parse_stm8_type1(asm_context, instr, n);
      }
        else
      {
        int size=parse_stm8_type1(asm_context, instr, n);
        token_type=get_token(asm_context, token, TOKENLEN);
        if (IS_NOT_TOKEN(token,','))
        {
          print_error_unexp(token, asm_context);
          return -1;
        }
        token_type=get_token(asm_context, token, TOKENLEN);
        if (strcasecmp(token, "a")!=0)
        {
          print_error_unexp(token, asm_context);
          return -1;
        }
        return size;
      }
    }
  }

  for (n=0; n<16; n++)
  {
    if (stm8_type2[n]==NULL) { continue; }
    if (strcmp(stm8_type2[n], instr_case)==0)
    {
    }
  }

#if 0
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
    add_bin8(asm_context, 0, IS_OPCODE);
    return 4;
  }
#endif


  printf("Error: Unknown instruction '%s'  at %s:%d\n", instr, asm_context->filename, asm_context->line);

  return -1;
}


