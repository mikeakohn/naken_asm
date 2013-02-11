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

#include "asm_680x0.h"
#include "asm_common.h"
#include "assembler.h"
#include "disasm_arm.h"
#include "get_tokens.h"
#include "eval_expression.h"
#include "table_805x.h"

enum
{
  OPERAND_A_REG,
  OPERAND_D_REG,
  OPERAND_A_INDEXED,
  OPERAND_DATA,
  OPERAND_NUM,
};

struct _operand
{
  int value;
  int type;
};

static int get_register_d_680x0(char *token)
{
  if (token[0]!='r' && token[0]!='R') return -1;
  if (token[1]>='0' && token[1]<='7' && token[2]==0)
  {
    return token[1]-'0';
  }

  return -1;
}

int parse_instruction_680x0(struct _asm_context *asm_context, char *instr)
{
char instr_lower_mem[TOKENLEN];
char *instr_lower=instr_lower_mem;
char token[TOKENLEN];
struct _operand operands[3];
int operand_count=0;
int token_type;
//int matched=0;
int num;
//int n,r;
int count=1;

  lower_copy(instr_lower, instr);

  memset(&operands, 0, sizeof(operands));
  while(1)
  {
    token_type=get_token(asm_context, token, TOKENLEN);
    if (token_type==TOKEN_EOL || token_type==TOKEN_EOF)
    {
      if (operand_count!=0)
      {
        print_error_unexp(token, asm_context);
        return -1;
      }
      break;
    }

    num=get_register_d_680x0(token);
    if (num!=-1)
    {
      operands[operand_count].type=OPERAND_D_REG;
      operands[operand_count].value=num;
    }
      else
    if (token_type==TOKEN_POUND)
    {
      token_type=get_token(asm_context, token, TOKENLEN);
      if (token_type!=TOKEN_NUMBER)
      {
        print_error_unexp(token, asm_context);
        return -1;
      }
      operands[operand_count].type=OPERAND_DATA;
      operands[operand_count].value=atoi(token);
    }
      else
    if (token_type==TOKEN_NUMBER)
    {
      operands[operand_count].type=OPERAND_NUM;
      operands[operand_count].value=atoi(token);

      token_type=get_token(asm_context, token, TOKENLEN);
      if (IS_TOKEN(token,'.'))
      {
        token_type=get_token(asm_context, token, TOKENLEN);
        if (token_type!=TOKEN_NUMBER)
        {
          print_error_unexp(token, asm_context);
          return -1;
        }

        num=atoi(token);
        if (num<0 || num>7)
        {
          printf("Error: bit address out of range at %s:%d\n", asm_context->filename, asm_context->line);
          return -1;
        }

        if (operands[operand_count].value>=0x20 &&
            operands[operand_count].value<=0x2f)
        {
          operands[operand_count].value-=0x20;
          operands[operand_count].value<<=3;
        }
          else
        if (operands[operand_count].value>=0x80 &&
            operands[operand_count].value<=0x8f)
        {
          operands[operand_count].value-=0x80;
          operands[operand_count].value<<=3;
          operands[operand_count].value|=128;
        }
          else
        {
          printf("Error: bit address out of range at %s:%d\n", asm_context->filename, asm_context->line);
          return -1;
        }

        operands[operand_count].value|=num;
      }
        else
      {
        pushback(asm_context, token, token_type);
      }
    }
      else
    {
      print_error_unexp(token, asm_context);
      return -1;
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

#ifdef DEBUG
printf("-----\n");
int n;
for (n=0; n<operand_count; n++)
{
printf("[%d %d]", operands[n].type, operands[n].value);
}
printf("\n");
#endif

#if 0
  for (n=0; n<256; n++)
  {
  }

  if (n==256)
  {
    if (matched==1)
    {
      printf("Error: Unknown operands combo for '%s' at %s:%d.\n", instr, asm_context->filename, asm_context->line);
    }
      else
    {
      printf("Error: Unknown instruction '%s' at %s:%d.\n", instr, asm_context->filename, asm_context->line);
    }
  }
#endif

  return count; 
}


