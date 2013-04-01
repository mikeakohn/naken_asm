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
  OPERAND_NONE=0,
  OPERAND_NUMBER,
  OPERAND_REG,
  OPERAND_REG16,
  OPERAND_MINUS_REG16,
  OPERAND_REG16_PLUS,
};

enum
{
  REG16_X=0,
  REG16_Y,
  REG16_Z,
};

struct _operand
{
  int value;
  char type;
};

static int get_register_avr8(char *token)
{
int n;
int r;

  if (token[0]=='r' || token[0]=='R')
  {
    if (token[1]==0) return -1;

    r=0; n=1;
    while(1)
    {
      if (token[n]==0) return r;
      if (token[n]<'0' || token[n]>'9') return -1;
      r=(r*10)+(token[n]-'0');
      n++;
    }
  }

  return -1;
}

int parse_instruction_avr8(struct _asm_context *asm_context, char *instr)
{
char token[TOKENLEN];
int token_type;
char instr_case[TOKENLEN];
struct _operand operands[2];
int operand_count=0;
int n,num;

  lower_copy(instr_case, instr);

  //pushback(asm_context, token, token_type);

  while(1)
  {
    token_type=get_token(asm_context, token, TOKENLEN);
    if (token_type==TOKEN_EOL || token_type==TOKEN_EOF)
    {
      break;
    }

    if (operand_count>2)
    {
      print_error_opcount(instr, asm_context);
      return -1;
    }

    if ((n=get_register_avr8(token))!=-1)
    {
      operands[operand_count].type=OPERAND_REG;
      operands[operand_count].value=n;
    }
      else
    if (IS_TOKEN(token,'x') || IS_TOKEN(token,'X'))
    {
      operands[operand_count].type=OPERAND_REG16;
      operands[operand_count].value=REG16_X;
      token_type=get_token(asm_context, token, TOKENLEN);
      if (IS_TOKEN(token,'+')) { operands[operand_count].type=OPERAND_REG16_PLUS; }
      else { pushback(asm_context, token, token_type); }
    }
      else
    if (IS_TOKEN(token,'y') || IS_TOKEN(token,'Y'))
    {
      operands[operand_count].type=OPERAND_REG16;
      operands[operand_count].value=REG16_Y;
      token_type=get_token(asm_context, token, TOKENLEN);
      if (IS_TOKEN(token,'+')) { operands[operand_count].type=OPERAND_REG16_PLUS; }
      else { pushback(asm_context, token, token_type); }
    }
      else
    if (IS_TOKEN(token,'z') || IS_TOKEN(token,'Z'))
    {
      operands[operand_count].type=OPERAND_REG16;
      operands[operand_count].value=REG16_Z;
      token_type=get_token(asm_context, token, TOKENLEN);
      if (IS_TOKEN(token,'+')) { operands[operand_count].type=OPERAND_REG16_PLUS; }
      else { pushback(asm_context, token, token_type); }
    }
      else
    if (IS_TOKEN(token,'-'))
    {
      operands[operand_count].type=OPERAND_MINUS_REG16;
      token_type=get_token(asm_context, token, TOKENLEN);
      if (IS_TOKEN(token,'x') || IS_TOKEN(token,'X'))
      {
        operands[operand_count].value=REG16_X;
      }
        else
      if (IS_TOKEN(token,'y') || IS_TOKEN(token,'Y'))
      {
        operands[operand_count].value=REG16_Y;
      }
        else
      if (IS_TOKEN(token,'z') || IS_TOKEN(token,'Z'))
      {
        operands[operand_count].value=REG16_Z;
      }
        else
      {
        //double pushback, or?
        pushback(asm_context, token, token_type);
        if (asm_context->pass==1)
        {
          eat_operand(asm_context);
          num=0;
        }
          else
        if (eval_expression(asm_context, &num)!=0)
        {
          print_error_illegal_expression(instr, asm_context);
          return -1;
        }

        operands[operand_count].type=OPERAND_NUMBER;
        operands[operand_count].value=-num;
      }
    }
      else
    {
      pushback(asm_context, token, token_type);

      if (eval_expression(asm_context, &num)!=0)
      {
        if (asm_context->pass==1)
        {
          eat_operand(asm_context);
        }
          else
        {
          print_error_illegal_expression(instr, asm_context);
          return -1;
        }
      }

      operands[operand_count].type=OPERAND_NUMBER;
      operands[operand_count].value=num;
    }

    operand_count++;
    token_type=get_token(asm_context, token, TOKENLEN);
    if (token_type==TOKEN_EOL) break;
    if (IS_NOT_TOKEN(token,',') || operand_count==3)
    {
      print_error_unexp(token, asm_context);
      return -1;
    }
  }

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



