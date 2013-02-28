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
#include "asm_68hc08.h"
#include "assembler.h"
#include "table_68hc08.h"
#include "get_tokens.h"
#include "eval_expression.h"

extern struct _m68hc08_table m68hc08_table[];
extern struct _m68hc08_16_table m68hc08_16_table[];

enum
{
  OPERAND_NONE,
  OPERAND_NUM8,
  OPERAND_NUM16,
  OPERAND_ADDRESS,
  OPERAND_X,
  OPERAND_X_PLUS,
};

struct _operands
{
  int value;
  char type;
  char error;
};

int parse_instruction_68hc08(struct _asm_context *asm_context, char *instr)
{
char token[TOKENLEN];
int token_type;
char instr_case[TOKENLEN];
struct _operands operands[3];
int operand_count=0;
//int address_size=0;
int opcode=-1;
int size=-1;
int matched=0;
int num;
int n;

  lower_copy(instr_case, instr);
  memset(operands, 0, sizeof(operands));

  while(1)
  {
    token_type=get_token(asm_context, token, TOKENLEN);
    if (token_type==TOKEN_EOL || token_type==TOKEN_EOF) { break; }

    if (token_type==TOKEN_POUND)
    {
      operands[operand_count].type=OPERAND_NUM8;

      if (eval_expression(asm_context, &num)!=0)
      {
        if (asm_context->pass==1)
        {
          eat_operand(asm_context);
          operands[operand_count].error=1;
        }
          else
        {
          print_error_illegal_expression(instr, asm_context);
          return -1;
        }

        num=0xffff;
      }

      if (num<-32768 || num>65535)
      {
        print_error_range("Constant", -32768, 65535, asm_context);
        return -1;
      }

      if (num<-128 || num>255)
      {
        operands[operand_count].type=OPERAND_NUM16;
      }

      operands[operand_count].value=num;
    }
      else
    if (strcasecmp(token, "x")==0)
    {
      token_type=get_token(asm_context, token, TOKENLEN);
      if (IS_TOKEN(token,'+'))
      {
        operands[operand_count].type=OPERAND_X_PLUS;
      }
        else
      {
        operands[operand_count].type=OPERAND_X;
        pushback(asm_context, token, token_type);
      }
    }
      else
    if (strcasecmp(token, ",")==0)
    {
      // operand type should be none
      pushback(asm_context, token, token_type);
    }
      else
    {
      operands[operand_count].type=OPERAND_ADDRESS;

      if (eval_expression(asm_context, &num)!=0)
      {
        if (asm_context->pass==1)
        {
          eat_operand(asm_context);
          operands[operand_count].error=1;
        }
          else
        {
          print_error_illegal_expression(instr, asm_context);
          return -1;
        }

        num=0xffff;
      }

      operands[operand_count].value=num;
    }

    operand_count++;

    token_type=get_token(asm_context, token, TOKENLEN);
    if (token_type==TOKEN_EOL || token_type==TOKEN_EOF) { break; }

    if (expect_token_s(asm_context,",")!=0) { return -1; }
  }

  // Done parsing, now assemble.

  if (asm_context->pass==2)
  {
    opcode=memory_read_m(&asm_context->memory, asm_context->address);
  }

  for(n=0; n<256; n++)
  {
    if (m68hc08_table[n].instr==NULL) { continue; }
    if (strcmp(m68hc08_table[n].instr, instr_case)==0)
    {
      matched=1;
      if (asm_context->pass==2 && n!=opcode) { continue; }
      if (m68hc08_table[n].operand_type==CPU08_OP_NONE &&
          operand_count==0)
      {
        add_bin8(asm_context, n, IS_OPCODE);
        return 1;
      }
        else
      if (operand_count==1)
      {
        if (m68hc08_table[n].operand_type==CPU08_OP_NUM8 &&
            operands[0].type==OPERAND_NUM8)
        {
          if (size!=-1)
          {
            // If we previously wrote a 16 bit version, we can reverse it
            // and use a smaller, better version.
            asm_context->address-=size;
          }
          add_bin8(asm_context, n, IS_OPCODE);
          add_bin8(asm_context, operands[0].value, IS_OPCODE);
          return 2;
        }
          else
        if (m68hc08_table[n].operand_type==CPU08_OP_NUM16 &&
            (operands[0].type==OPERAND_NUM16 || operands[0].type==OPERAND_NUM8))
        {
          // 8 bit num can fit in here too, but better if it fits in NUM8
          add_bin8(asm_context, n, IS_OPCODE);
          add_bin8(asm_context, operands[0].value>>8, IS_OPCODE);
          add_bin8(asm_context, operands[0].value&0xff, IS_OPCODE);
          if (asm_context->pass==2) { return 3; }
          size=3;
        }
      }
    }
  }

  if (size!=-1) { return size; }

  if (asm_context->pass==2)
  {
    opcode=(memory_read_m(&asm_context->memory, asm_context->address)<<8)|memory_read_m(&asm_context->memory, asm_context->address+1);
  }

  n=0;
  while(m68hc08_16_table[opcode].instr!=NULL)
  {
    if (strcmp(m68hc08_16_table[n].instr, instr_case)==0)
    {
      if (asm_context->pass==2 &&
          m68hc08_16_table[n].opcode!=opcode)
      {
        continue;
      }

      matched=1;
    }
  }

  if (matched==1)
  {
    printf("Error: Unknown flag/operands combo for '%s' at %s:%d.\n", instr, asm_context->filename, asm_context->line);
  }
    else
  {
    print_error_unknown_instr(instr, asm_context);
  }

  print_error_unknown_instr(instr, asm_context);

  return -1;
}



