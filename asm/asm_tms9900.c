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
#include "asm_tms9900.h"
#include "assembler.h"
#include "table_tms9900.h"
#include "get_tokens.h"
#include "eval_expression.h"

enum
{
  OPERAND_REGISTER,
  OPERAND_REGISTER_INDIRECT,
  OPERAND_REGISTER_INDIRECT_INC,
  OPERAND_ADDRESS,
  OPERAND_TABLE,
  OPERAND_NUMBER,
};

struct _operand
{
  char type;
  char reg;
  int value;
};

static int get_register_tms9900(char *token)
{
  if (token[0]=='r' || token[0]=='R')
  {
    if (token[2]==0 && (token[1]>='0' && token[1]<='9'))
    {
      return token[1]-'0';
    }
      else
    if (token[3]==0 && token[1]=='1' && (token[2]>='0' && token[2]<='5'))
    {
      return 10+(token[2]-'0');
    }
  }

  return -1;
}

int parse_instruction_tms9900(struct _asm_context *asm_context, char *instr)
{
char token[TOKENLEN];
int token_type;
char instr_case[TOKENLEN];
struct _operand operands[2];
int operand_count;
int word_count=1;
int opcode;
int n;

  lower_copy(instr_case, instr);
  operand_count=0;
  memset(operands, 0, sizeof(operands));

  while(1)
  {
    token_type=get_token(asm_context, token, TOKENLEN);

    if (token_type==TOKEN_EOL || token_type==TOKEN_EOF)
    {
      break;
    }

    if (operand_count>=2)
    {
      print_error_opcount(instr, asm_context);
      return -1;
    }

    if ((n=get_register_tms9900(token))!=-1)
    {
      operands[operand_count].type=OPERAND_REGISTER;
      operands[operand_count].reg=n;
    }
      else
    if (IS_TOKEN(token,'*'))
    {
      token_type=get_token(asm_context, token, TOKENLEN);
      n=get_register_tms9900(token);
      if (n==-1)
      {
        print_error_unexp(token, asm_context);
        return -1;
      }
      operands[operand_count].type=OPERAND_REGISTER_INDIRECT;
      operands[operand_count].reg=n;

      token_type=get_token(asm_context, token, TOKENLEN);
      if (IS_TOKEN(token,'+'))
      {
        operands[operand_count].type=OPERAND_REGISTER_INDIRECT_INC;
      }
        else
      {
        pushback(asm_context, token, token_type);
      }
    }
      else
    if (IS_TOKEN(token,'@'))
    {
      operands[operand_count].type=OPERAND_ADDRESS;
      word_count++;

      if (asm_context->pass==1)
      {
        eat_operand(asm_context);
        operands[operand_count].value=0;
      }
        else
      {
        if (eval_expression(asm_context, &n)!=0)
        {
          print_error_illegal_expression(instr, asm_context);
          return -1;
        }

        if (n<0 || n>65535)
        {
          print_error_range("Address", 0, 65535, asm_context);
          return -1;
        }

        operands[operand_count].value=n;

        token_type=get_token(asm_context, token, TOKENLEN);
        if (IS_TOKEN(token,'('))
        {
          operands[operand_count].type=OPERAND_TABLE;
          token_type=get_token(asm_context, token, TOKENLEN);
          n=get_register_tms9900(token);
          if (n==-1)
          {
            print_error_unexp(token, asm_context);
            return -1;
          }
          if (n==0)
          {
            printf("Error: r0 cannot be used in a table at %s:%d.\n", asm_context->filename, asm_context->line);
            return -1;
          }
          if (expect_token_s(asm_context,")")!=0) { return -1; }
        }
          else
        {
          pushback(asm_context, token, token_type);
        }
      }
    }
      else
    {
      word_count++;

      if (asm_context->pass==1)
      {
        eat_operand(asm_context);
        n=0;
      }
        else
      {
        if (eval_expression(asm_context, &n)!=0)
        {
          print_error_illegal_expression(instr, asm_context);
          return -1;
        }
      }

      operands[operand_count].type=OPERAND_NUMBER;
      operands[operand_count].value=n;
    }

    operand_count++;
    token_type=get_token(asm_context, token, TOKENLEN);
    if (token_type==TOKEN_EOL || token_type==TOKEN_EOF) break;
    if (IS_NOT_TOKEN(token,',') || operand_count==3)
    {
      print_error_unexp(token, asm_context);
      return -1;
    }
  }

  if (asm_context->pass==1)
  {
    for (n=0; n<word_count; n++)
    {
      add_bin(asm_context, 0, IS_OPCODE);
    }

    return word_count*2;
  }

  n=0;
  while(table_tms9900[n].instr!=NULL)
  {
    if (strcmp(table_tms9900[n].instr,instr_case)==0)
    {
      switch(table_tms9900[n].type)
      {
        case OP_DUAL:
          if (operand_count!=2)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }
          opcode=table_tms9900[n].opcode|(operands[0].reg<<6)|operands[1].reg;
          add_bin(asm_context, opcode, IS_OPCODE);
          break;
        case OP_DUAL_MULTIPLE:
        case OP_XOP:
        case OP_SINGLE:
        case OP_CRU_MULTIBIT:
        case OP_CRU_SINGLEBIT:
        case OP_JUMP:
        case OP_SHIFT:
        case OP_IMMEDIATE:
        case OP_INT_REG_LD:
        case OP_INT_REG_ST:
        case OP_RTWP:
        case OP_EXTERNAL:
          break;
      }
    }

    n++;
  }

  print_error_unknown_instr(instr, asm_context);

  return -1;
}



