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
#include "table_tms1000.h"
#include "get_tokens.h"
#include "eval_expression.h"

static char *tmsinstr_1[] = { "sbit", "rbit", "tbit1", "ldx" };
static char *tmsinstr_2[] = { "tcy", "ynec", "tcmiy", "alec", "ldp" };
static char *tms_branch[] = { "br", "call" };

int parse_instruction_tms1000(struct _asm_context *asm_context, char *instr)
{
char token[TOKENLEN];
int token_type;
char instr_case[TOKENLEN];
int n;

  lower_copy(instr_case, instr);

  n=0;
  while(table_tms1000[n].instr!=NULL)
  {
    if (strcmp(instr_case, table_tms1000[n].instr)==0 &&
        table_tms1000[n].op1000!=0xffff)
    {
      add_bin8(asm_context, table_tms1000[n].op1000, IS_OPCODE);
      return 1;
    }
    n++;
  }

  for (n=0; n<4; n++)
  {
    if (strcmp(instr_case, tmsinstr_1[n])==0)
    {
      token_type=get_token(asm_context, token, TOKENLEN);
      if (token_type!=TOKEN_NUMBER)
      {
        if (asm_context->pass==1) { return 1; }
        print_error_unexp(token, asm_context);
        return -1;
      }

      int num=atoi(token);
      if (num<0 || num>3)
      {
        print_error_range("Constant", 0, 3, asm_context);
        return -1;
      }

      add_bin8(asm_context, ((0xc+n)<<2)|num, IS_OPCODE);

      return 1;
    }
  }

  for (n=0; n<5; n++)
  {
    if (strcmp(instr_case, tmsinstr_2[n])==0)
    {
      token_type=get_token(asm_context, token, TOKENLEN);
      if (token_type!=TOKEN_NUMBER)
      {
        if (asm_context->pass==1) { return 1; }
        print_error_unexp(token, asm_context);
        return -1;
      }

      int num=atoi(token);
      if (num<0 || num>15)
      {
        print_error_range("Constant", 0, 3, asm_context);
        return -1;
      }

      if (n<4)
      {
        add_bin8(asm_context, ((0x4+n)<<4)|num, IS_OPCODE);
      }
        else
      {
        add_bin8(asm_context, 0x10|num, IS_OPCODE);
      }

      return 1;
    }
  }

  for (n=0; n<2; n++)
  {
    if (strcmp(instr_case, tms_branch[n])==0)
    {
      int num=0;
      if (eval_expression(asm_context, &num)!=0)
      {
        if (asm_context->pass==2)
        {
          print_error_illegal_expression(instr, asm_context);
          return -1;
        }

        eat_operand(asm_context);
        num=asm_context->address;
      }

      num=num-(asm_context->address+1);

      if (num<-32 || num>31)
      {
        print_error_range("Offset", -32, 31, asm_context);
        return -1;
      }

      add_bin8(asm_context, (0x80|(n<<6))|(num&0x3f), IS_OPCODE);

      return 1;
    }
  }

  print_error_unknown_instr(instr, asm_context);

  return -1;
}

int parse_instruction_tms1100(struct _asm_context *asm_context, char *instr)
{
char token[TOKENLEN];
int token_type;
char instr_case[TOKENLEN];
int n;

  lower_copy(instr_case, instr);

  n=0;
  while(table_tms1000[n].instr!=NULL)
  {
    if (strcmp(instr_case, table_tms1000[n].instr)==0 &&
        table_tms1000[n].op1100!=0xffff)
    {
      add_bin8(asm_context, table_tms1000[n].op1100, IS_OPCODE);
      return 1;
    }
    n++;
  }

  for (n=0; n<4; n++)
  {
    if (strcmp(instr_case, tmsinstr_1[n])==0)
    {
      token_type=get_token(asm_context, token, TOKENLEN);
      if (token_type!=TOKEN_NUMBER)
      {
        if (asm_context->pass==1) { return 1; }
        print_error_unexp(token, asm_context);
        return -1;
      }

      int num=atoi(token);

      if (n==3)
      {
        if (num<0 || num>7)
        {
          print_error_range("Constant", 0, 7, asm_context);
          return -1;
        }

        add_bin8(asm_context, ((0x5)<<3)|num, IS_OPCODE);
      }
        else
      {
        if (num<0 || num>3)
        {
          print_error_range("Constant", 0, 3, asm_context);
          return -1;
        }

        add_bin8(asm_context, ((0xc+n)<<2)|num, IS_OPCODE);
      }

      return 1;
    }
  }

  for (n=0; n<5; n++)
  {
    if (n==3) { continue; }
    if (strcmp(instr_case, tmsinstr_2[n])==0)
    {
      token_type=get_token(asm_context, token, TOKENLEN);
      if (token_type!=TOKEN_NUMBER)
      {
        if (asm_context->pass==1) { return 1; }
        print_error_unexp(token, asm_context);
        return -1;
      }

      int num=atoi(token);
      if (num<0 || num>15)
      {
        print_error_range("Constant", 0, 3, asm_context);
        return -1;
      }

      if (n<4)
      {
        add_bin8(asm_context, ((0x4+n)<<4)|num, IS_OPCODE);
      }
        else
      {
        add_bin8(asm_context, 0x10|num, IS_OPCODE);
      }

      return 1;
    }
  }

  for (n=0; n<2; n++)
  {
    if (strcmp(instr_case, tms_branch[n])==0)
    {
      int num=0;
      if (eval_expression(asm_context, &num)!=0)
      {
        if (asm_context->pass==2)
        {
          print_error_illegal_expression(instr, asm_context);
          return -1;
        }

        eat_operand(asm_context);
        num=asm_context->address;
      }

      num=num-(asm_context->address+1);

      if (num<-32 || num>31)
      {
        print_error_range("Offset", -32, 31, asm_context);
        return -1;
      }

      add_bin8(asm_context, (0x80|(n<<6))|(num&0x3f), IS_OPCODE);

      return 1;
    }
  }

  print_error_unknown_instr(instr, asm_context);

  return -1;
}


