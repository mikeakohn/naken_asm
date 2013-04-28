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
#include "asm_thumb.h"
#include "assembler.h"
#include "table_thumb.h"
#include "get_tokens.h"
#include "eval_expression.h"

enum
{
  OPERAND_NONE,
  OPERAND_REGISTER,
  OPERAND_NUMBER,
  OPERAND_TWO_REG_IN_BRACKETS,      // [ Rb, Ro ]
  OPERAND_REG_AND_NUM_IN_BRACKETS,  // [ Rb, #IMM ]
  //OPERAND_PC_AND_REG_IN_BRACKETS,   // [ PC, Rb ]
  //OPERAND_PC_AND_NUM_IN_BRACKETS,   // [ PC, #IMM ]
};

struct _operand
{
  int value;
  int type;
  int second_value;
};

// FIXME - This is the same as get_register_arm()
static int get_register_thumb(char *token)
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

  if (strcasecmp("sp", token)==0) { return 13; }
  if (strcasecmp("lr", token)==0) { return 14; }
  if (strcasecmp("pc", token)==0) { return 15; }

  return -1;
}

static int check_reg_lower(struct _asm_context *asm_context, int value)
{
  if (value>7)
  {
    print_error_range("Register", 0, 7, asm_context);
    return -1;
  }

  return 0;
}

int parse_instruction_thumb(struct _asm_context *asm_context, char *instr)
{
char token[TOKENLEN];
int token_type;
char instr_case[TOKENLEN];
struct _operand operands[3];
int operand_count=0;
int matched=0;
int num;
int n;

  lower_copy(instr_case, instr);

  //token_type=get_token(asm_context, token, TOKENLEN);
  //pushback(asm_context, token, token_type);

  memset(&operands, 0, sizeof(operands));
  while(1)
  {
    token_type=get_token(asm_context, token, TOKENLEN);
    if (token_type==TOKEN_EOL || token_type==TOKEN_EOF)
    {
      break;
    }

    if (operand_count>=3)
    {
      print_error_opcount(instr, asm_context);
      return -1;
    }

    if ((num=get_register_thumb(token))!=-1)
    {
      operands[operand_count].type=OPERAND_REGISTER;
      operands[operand_count].value=num;
    }
      else
    if (token_type==TOKEN_POUND)
    {
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
      else
    if (IS_TOKEN(token,'['))
    {
      token_type=get_token(asm_context, token, TOKENLEN);
#if 0
      if (strcasecmp(token,"pc")==0)
      {
        operands[operand_count].type=OPERAND_PC_AND_REG_IN_BRACKETS;
      }
        else
#endif
      if ((num=get_register_thumb(token))!=-1)
      {
        operands[operand_count].type=OPERAND_TWO_REG_IN_BRACKETS;
        operands[operand_count].value=num;
      }
        else
      {
        print_error_unexp(token, asm_context);
        return -1;
      }

      if (expect_token_s(asm_context,",")!=0) { return -1; }

      token_type=get_token(asm_context, token, TOKENLEN);

      if ((num=get_register_thumb(token))!=-1)
      {
        operands[operand_count].second_value=num;
      }
        else
      if (token_type==TOKEN_POUND)
      {
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

        operands[operand_count].second_value=num;
        operands[operand_count].type++;
      }
        else
      {
        print_error_unexp(token, asm_context);
        return -1;
      }

      if (expect_token_s(asm_context,"]")!=0) { return -1; }
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
  while(table_thumb[n].instr!=NULL)
  {
    if (strcmp(table_thumb[n].instr,instr_case)==0)
    {
      matched=1;

      switch(table_thumb[n].type)
      {
        case OP_SHIFT:
          if (operand_count!=3 &&
              operands[0].type==OPERAND_REGISTER &&
              operands[1].type==OPERAND_REGISTER &&
              operands[2].type==OPERAND_NUMBER)
          {
            if (check_reg_lower(asm_context, operands[0].value)==-1) { return -1; }
            if (check_reg_lower(asm_context, operands[1].value)==-1) { return -1; }
            if (operands[2].value<0 || operands[2].value>32)
            {
              print_error_range("Offset", 0, 31, asm_context);
              return -1;
            }
            add_bin16(asm_context, table_thumb[n].opcode, IS_OPCODE);
            return 2;
          }
        default:
          break;
      }
    }

    n++;
  }

  if (matched==1)
  {
    printf("Error: Unknown operands combo for '%s' at %s:%d.\n", instr, asm_context->filename, asm_context->line);
  }
    else
  {
    printf("Error: Unknown instruction '%s' at %s:%d.\n", instr, asm_context->filename, asm_context->line);
  }

  return -1;
}



