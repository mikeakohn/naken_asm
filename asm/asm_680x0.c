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
#include "table_680x0.h"

extern struct _table_680x0_no_operands table_680x0_no_operands[];
extern struct _table_680x0 table_680x0[];

enum
{
  OPERAND_D_REG,
  OPERAND_A_REG,
  OPERAND_A_REG_INDEX,
  OPERAND_A_REG_INDEX_PLUS,
  OPERAND_A_REG_INDEX_MINUS,
  OPERAND_IMMEDIATE,
  OPERAND_ADDRESS,
};

enum
{
  SIZE_NONE=-1,
  SIZE_B,
  SIZE_W,
  SIZE_L,
};

struct _operand
{
  int value;
  int type;
};

static int get_register_d_680x0(char *token)
{
  if (token[0]!='d' && token[0]!='D') return -1;
  if (token[1]>='0' && token[1]<='7' && token[2]==0)
  {
    return token[1]-'0';
  }

  return -1;
}

static int get_register_a_680x0(char *token)
{
  if (strcasecmp(token, "sp")==0) return 7;
  if (token[0]!='d' && token[0]!='D') return -1;
  if (token[1]>='0' && token[1]<='7' && token[2]==0)
  {
    return token[1]-'0';
  }

  return -1;
}

int write_single_ea(struct _asm_context *asm_context, char *instr, struct _operand *operands, int operand_count, int opcode, int size)
{
  if (operand_count!=1)
  {
    print_error_opcount(instr, asm_context);
    return -1;
  }

  switch(operands[0].type)
  {
    case OPERAND_D_REG:
    case OPERAND_A_REG:
    case OPERAND_A_REG_INDEX:
    case OPERAND_A_REG_INDEX_PLUS:
    case OPERAND_A_REG_INDEX_MINUS:
      add_bin(asm_context, opcode|(size<<6)|(operands[0].type<<3)|operands[0].value, IS_OPCODE);
      return 2;
    case OPERAND_IMMEDIATE:
    default:
      print_error_illegal_operands(instr, asm_context);
      return -1;
  }
}

int parse_instruction_680x0(struct _asm_context *asm_context, char *instr)
{
char token[TOKENLEN];
int token_type;
char instr_case_c[TOKENLEN];
char *instr_case=instr_case_c;
struct _operand operands[3];
int operand_count=0;
int operand_size=SIZE_NONE;
//int matched=0;
int num;
//int n,r;
int count=1;
int n;

  lower_copy(instr_case, instr);
  memset(operands, 0, sizeof(operands));

#if 0
  if (strcmp("bhs", instr_case)==0) { instr_case="bcc"; }
  else if (strcmp("blo", instr_case)==0) { instr_case="bcs"; }
#endif

  memset(&operands, 0, sizeof(operands));
  while(1)
  {
    token_type=get_token(asm_context, token, TOKENLEN);
    if (token_type==TOKEN_EOL || token_type==TOKEN_EOF)
    {
#if 0
      if (operand_count!=0)
      {
        print_error_unexp(token, asm_context);
        return -1;
      }
#endif
      break;
    }

    if (IS_TOKEN(token, '.') && operand_count==0 && operand_size==SIZE_NONE)
    {
      token_type=get_token(asm_context, token, TOKENLEN);
      if (strcasecmp(token, "b")==0) { operand_size=SIZE_B; }
      else if (strcasecmp(token, "w")==0) { operand_size=SIZE_W; }
      else if (strcasecmp(token, "l")==0) { operand_size=SIZE_L; }
      else
      {
        print_error_unexp(token, asm_context);
        return -1;
      }

      continue;
    }

    if ((num=get_register_d_680x0(token))!=-1)
    {
      operands[operand_count].type=OPERAND_D_REG;
      operands[operand_count].value=num;
    }
      else
    if ((num=get_register_a_680x0(token))!=-1)
    {
      operands[operand_count].type=OPERAND_A_REG;
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
          //operands[operand_count].error=1;
        }
          else
        {
          print_error_illegal_expression(instr, asm_context);
          return -1;
        }
      }

      operands[operand_count].type=OPERAND_IMMEDIATE;
      operands[operand_count].value=num;
    }
      else
    if (IS_TOKEN(token,'-'))
    {
      if (expect_token_s(asm_context,"(")!=0) { return -1; }
      if ((num=get_register_a_680x0(token))!=-1)
      {
        print_error_unexp(token, asm_context);
        return -1;
      }
      if (expect_token_s(asm_context,")")!=0) { return -1; }

      operands[operand_count].type=OPERAND_A_REG_INDEX_MINUS;
      operands[operand_count].value=num;
    }
      else
    if (IS_TOKEN(token,'('))
    {
      if ((num=get_register_a_680x0(token))!=-1)
      {
        print_error_unexp(token, asm_context);
        return -1;
      }
      operands[operand_count].value=num;
      if (expect_token_s(asm_context,")")!=0) { return -1; }
      token_type=get_token(asm_context, token, TOKENLEN);
      if (IS_TOKEN(token,'+'))
      {
        operands[operand_count].type=OPERAND_A_REG_INDEX_PLUS;
      }
        else
      {
        operands[operand_count].type=OPERAND_A_REG_INDEX;
        pushback(asm_context, token, token_type);
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

      operands[operand_count].type=OPERAND_ADDRESS;
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

#ifdef DEBUG
printf("-----\n");
int n;
for (n=0; n<operand_count; n++)
{
printf("[%d %d]", operands[n].type, operands[n].value);
}
printf("\n");
#endif

  n=0;
  while(table_680x0_no_operands[n].instr!=NULL)
  {
    if (strcmp(table_680x0_no_operands[n].instr, instr_case)==0)
    {
      if (operand_size!=SIZE_NONE)
      {
        printf("Error: %s doesn't take a size attribute at %s:%d\n", instr, asm_context->filename, asm_context->line);
        return -1;
      }

      if (operand_count!=0)
      {
        print_error_opcount(instr, asm_context);
        return -1;
      }

      add_bin(asm_context, table_680x0_no_operands[n].opcode, IS_OPCODE);
      return 2;
    }
    n++;
  }

  n=0;
  while(table_680x0[n].instr!=NULL)
  {
    if (strcmp(table_680x0[n].instr, instr_case)==0)
    {
      switch(table_680x0[n].type)
      {
        case OP_SINGLE_EA:
          return write_single_ea(asm_context, instr, operands, operand_count, table_680x0[n].opcode, operand_size);
        default:
          printf("Error: Unknown flag/operands combo for '%s' at %s:%d.\n", instr, asm_context->filename, asm_context->line);
          return -1;

      }
    }

    n++;
  }

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
  printf("Error: Unknown instruction '%s' at %s:%d.\n", instr, asm_context->filename, asm_context->line);

  return -1; 
}


