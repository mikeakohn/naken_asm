/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2012 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "asm_arm.h"
#include "asm_common.h"
#include "assembler.h"
#include "disasm_arm.h"
#include "get_tokens.h"
#include "eval_expression.h"

static char *arm_cond_a[16] =
{
  "eq", "ne", "cs", "ne",
  "mi", "pl", "vs", "vc",
  "hi", "ls", "ge", "lt",
  "gt", "le", "al", "nv"
};

enum
{
  OPERAND_REG,
  OPERAND_IMMEDIATE,
  OPERAND_INDEXED,
  OPERAND_SHIFT,
};


struct _operand
{
  int value;
  int type;
  int sub_type;
};

static int get_register_arm(char *token)
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

int parse_condition(char **instr_lower)
{
int cond;
char *instr=*instr_lower;

  for (cond=0; cond<16; cond++)
  {
    if (strncmp(instr, arm_cond_a[cond], 2)==0)
    { *instr_lower+=2; break; }
  }
  if (cond==16) { cond=14; }

  return cond;
}

int parse_instruction_arm(struct _asm_context *asm_context, char *instr)
{
struct _operand operands[3];
int operand_count;
char instr_lower_mem[TOKENLEN];
char *instr_lower=instr_lower_mem;
char token[TOKENLEN];
int token_type;
int n,cond,s=0;
int opcode=0;

  lower_copy(instr_lower, instr);
  memset(operands, 0, sizeof(operands));
  operand_count=0;

  // First parse instruction into the operands structures.
  while(1)
  {
    token_type=get_token(asm_context, token, TOKENLEN);
    if (token_type==TOKEN_EOL) { break; }

    n=get_register_arm(token);

    if (n!=-1)
    {
      operands[operand_count].value=n;
      operands[operand_count].type=OPERAND_REG;
    }
      else
    if (IS_TOKEN(token,'#'))
    {
      token_type=get_token(asm_context, token, TOKENLEN);
      if (token_type!=TOKEN_NUMBER)
      {
        print_error_unexp(token, asm_context);
        return -1;
      }
      operands[operand_count].value=atoi(token);
      operands[operand_count].type=OPERAND_IMMEDIATE;
    }
      else
    if (IS_TOKEN(token,'['))
    {
      operands[operand_count].type=OPERAND_INDEXED;
      token_type=get_token(asm_context, token, TOKENLEN);
      n=get_register_arm(token);
      if (n!=-1)
      {
        print_error_unexp(token, asm_context);
        return -1;
      }

      operands[operand_count].value=n;
      token_type=get_token(asm_context, token, TOKENLEN);

      if (IS_NOT_TOKEN(token,']'))
      {
        print_error_unexp(token, asm_context);
        return -1;
      }
    }
      else
    {
      for (n=0; n<4; n++)
      {
        if (strcasecmp(token, arm_alu_ops[n])==0)
        {
          token_type=get_token(asm_context, token, TOKENLEN);
          if (IS_NOT_TOKEN(token,'#'))
          {
            print_error_unexp(token, asm_context);
            return -1;
          }

          token_type=get_token(asm_context, token, TOKENLEN);
          if (token_type!=TOKEN_NUMBER)
          {
            print_error_unexp(token, asm_context);
            return -1;
          }

          operands[operand_count].value=atoi(token);
          operands[operand_count].type=OPERAND_SHIFT;
          operands[operand_count].sub_type=n;

          break;
        }
      }

      if (n==4)
      {
        print_error_unexp(token, asm_context);
        return -1;
      }
    }

    operand_count++;

    token_type=get_token(asm_context, token, TOKENLEN);
    if (token_type==TOKEN_EOL) { break; }
    if (IS_NOT_TOKEN(token,','))
    {
      print_error_unexp(token, asm_context);
      return -1;
    }

    if (operand_count==3)
    {
      print_error_unexp(token, asm_context);
      return -1;
    }
  }

  // Check for an ALU instruction
  for (n=0; n<16; n++)
  {
    if (strncmp(instr_lower, arm_alu_ops[n], 3)==0)
    {
      instr_lower+=3;

      cond=parse_condition(&instr_lower);

      if (instr_lower[0]=='s') { s=1; }

      if (operand_count<3 || operand_count>4 ||
          operands[0].type!=OPERAND_REG ||
          operands[1].type!=OPERAND_REG)
      {
        printf("Error: Illegal operands for '%s' at %s:%d\n", instr, asm_context->filename, asm_context->line);
      }

      opcode=(cond<<28)|ALU_OPCODE|(n<<21)|(s<<20);
      opcode|=operands[0].value<<12;
      opcode|=operands[1].value<<16;

      if (operands[2].type==OPERAND_IMMEDIATE)
      {
        if (operands[2].value<-128 || operands[2].value>255)
        {
          print_error("Constant larger than 8 bit.", asm_context);
          return -1;
        }

        opcode|=operands[2].value|(1<<25);

        if (operand_count==4)
        {
#if 0
fuck fuck fuck fuck fuck
          if (operands[4].type!=OPERAND_LSL)
          {
            print_error_unexp(token, asm_context);
            return -1;
          }
#endif

          opcode|=operands[2].value<<8;
        }
      }
        else
      if (operands[2].type==OPERAND_REG)
      {
      }
        else
      {
        print_error_unexp(token, asm_context);
        return -1;
      }

      add_bin32(asm_context, opcode, IS_OPCODE);
      return 4;

    }
  }

  // Check for SWI
  if (strncmp(instr_lower, "swi", 3)==0)
  {
    instr_lower+=3;
    cond=parse_condition(&instr_lower);

    if (*instr_lower==0)
    {
      if (operand_count!=0)
      {
        print_error_opcount(instr, asm_context);
        return -1;
      }

      add_bin32(asm_context, 0xf000000|(cond<<28), IS_OPCODE);
      return 4;
    }
  }

  printf("Error: Unknown instruction '%s' at %s:%d\n", instr, asm_context->filename, asm_context->line);

  return -1;
}


