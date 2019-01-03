/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2019 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "asm/common.h"
#include "asm/68hc08.h"
#include "common/assembler.h"
#include "common/eval_expression.h"
#include "common/tokens.h"
#include "table/68hc08.h"

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
  OPERAND_SP,
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
char instr_case_c[TOKENLEN];
char *instr_case=instr_case_c;
struct _operands operands[3];
int operand_count=0;
int offset;
//int address_size=0;
int opcode=-1;
int size=-1;
int matched=0;
int num;
int n;

  lower_copy(instr_case, instr);
  memset(operands, 0, sizeof(operands));

  if (strcmp("bhs", instr_case)==0) { instr_case="bcc"; }
  else if (strcmp("blo", instr_case)==0) { instr_case="bcs"; }
  else if (strcmp("lsl", instr_case)==0) { instr_case="asl"; }
  else if (strcmp("lslx", instr_case)==0) { instr_case="aslx"; }

  while(1)
  {
    token_type=tokens_get(asm_context, token, TOKENLEN);
    if (token_type==TOKEN_EOL || token_type==TOKEN_EOF) { break; }

    if (operand_count>=3)
    {
      print_error_opcount(instr, asm_context);
      return -1;
    }

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
      token_type=tokens_get(asm_context, token, TOKENLEN);
      if (IS_TOKEN(token,'+'))
      {
        operands[operand_count].type=OPERAND_X_PLUS;
      }
        else
      {
        operands[operand_count].type=OPERAND_X;
        tokens_push(asm_context, token, token_type);
      }
    }
      else
    if (strcasecmp(token, "sp")==0)
    {
      operands[operand_count].type=OPERAND_SP;
    }
      else
    if (strcasecmp(token, ",")==0 && operand_count==0)
    {
      // operand type should be none
      tokens_push(asm_context, token, token_type);
    }
      else
    {
      tokens_push(asm_context, token, token_type);
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

    token_type=tokens_get(asm_context, token, TOKENLEN);
    if (token_type==TOKEN_EOL || token_type==TOKEN_EOF) { break; }
    if (IS_NOT_TOKEN(token, ','))
    {
      print_error_unexp(token, asm_context);
      return -1;
    }
  }

  // Done parsing, now assemble.

#if 0
printf("---------- %s operand_count=%d\n", instr, operand_count);
for (n=0; n<operand_count; n++)
{
printf("%04x %d\n", operands[n].value, operands[n].type);
}
#endif

  int start=0;
  int end=256;
  if (asm_context->pass==2)
  {
    opcode=memory_read_m(&asm_context->memory, asm_context->address);
//printf("opcode=%02x address=%d\n", opcode, asm_context->address);
    start=opcode;
    end=opcode+1;
  }

//printf("line %d:  address=%d\n", asm_context->line, asm_context->address);

  for(n=start; n<end; n++)
  {
    if (m68hc08_table[n].instr==NULL) { continue; }
    if (strcmp(m68hc08_table[n].instr, instr_case)==0)
    {
      matched=1;
      //if (asm_context->pass==2 && n!=opcode) { continue; }
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
          size=3;
          if (asm_context->pass==2) { return size; }
        }
          else
        if (m68hc08_table[n].operand_type==CPU08_OP_OPR16 &&
            operands[0].type==OPERAND_ADDRESS)
        {
          // 8 bit num can fit in here too, but better if it fits in NUM8
          add_bin8(asm_context, n, IS_OPCODE);
          add_bin8(asm_context, operands[0].value>>8, IS_OPCODE);
          add_bin8(asm_context, operands[0].value&0xff, IS_OPCODE);
          size=3;
          if (asm_context->pass==2) { return size; }
        }
          else
        if (m68hc08_table[n].operand_type==CPU08_OP_OPR8 &&
            operands[0].type==OPERAND_ADDRESS &&
            operands[0].value<=0xff)
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
        if (m68hc08_table[n].operand_type==CPU08_OP_X &&
            operands[0].type==OPERAND_X)
        {
          add_bin8(asm_context, n, IS_OPCODE);
          return 1;
        }
          else
        if (m68hc08_table[n].operand_type==CPU08_OP_REL &&
            operands[0].type==OPERAND_ADDRESS)
        {
          if (asm_context->pass==1) { operands[0].value=asm_context->address; }
          offset=operands[0].value-(asm_context->address+2);
          if (offset<-128 || offset>127)
          {
            print_error_range("Offset", -128, 127, asm_context);
            return -1;
          }
          add_bin8(asm_context, n, IS_OPCODE);
          add_bin8(asm_context, (char)offset, IS_OPCODE);
          return 2;
        }
      }
        else
      if (operand_count==2)
      {
        if (m68hc08_table[n].operand_type==CPU08_OP_OPR8_OPR8 &&
            operands[0].type==OPERAND_ADDRESS &&
            operands[1].type==OPERAND_ADDRESS &&
            operands[0].value <= 0xff &&
            operands[1].value <= 0xff)
        {
          add_bin8(asm_context, n, IS_OPCODE);
          add_bin8(asm_context, operands[0].value, IS_OPCODE);
          add_bin8(asm_context, operands[1].value, IS_OPCODE);
          return 3;
        }
          else
        if (m68hc08_table[n].operand_type==CPU08_OP_NUM8_OPR8 &&
            operands[0].type==OPERAND_NUM8 &&
            operands[1].type==OPERAND_ADDRESS &&
            operands[1].value <= 0xff)
        {
          add_bin8(asm_context, n, IS_OPCODE);
          add_bin8(asm_context, operands[0].value, IS_OPCODE);
          add_bin8(asm_context, operands[1].value, IS_OPCODE);
          return 3;
        }
          else
        if (m68hc08_table[n].operand_type==CPU08_OP_NUM8_REL &&
            operands[0].type==OPERAND_NUM8 &&
            operands[1].type==OPERAND_ADDRESS)
        {
          if (asm_context->pass==1) { operands[1].value=asm_context->address; }
          offset=operands[1].value-(asm_context->address+3);
          if (offset<-128 || offset>127)
          {
            print_error_range("Offset", -128, 127, asm_context);
            return -1;
          }
          add_bin8(asm_context, n, IS_OPCODE);
          add_bin8(asm_context, operands[0].value, IS_OPCODE);
          add_bin8(asm_context, (char)offset, IS_OPCODE);
          return 3;
        }
          else
        if (m68hc08_table[n].operand_type==CPU08_OP_OPR16_X &&
            operands[0].type==OPERAND_ADDRESS &&
            operands[1].type==OPERAND_X)
        {
          // 8 bit num can fit in here too, but better if it fits in NUM8
          add_bin8(asm_context, n, IS_OPCODE);
          add_bin8(asm_context, operands[0].value>>8, IS_OPCODE);
          add_bin8(asm_context, operands[0].value&0xff, IS_OPCODE);
          size=3;
          if (asm_context->pass==2) { return size; }
        }
          else
        if (m68hc08_table[n].operand_type==CPU08_OP_OPR8_X &&
            operands[0].type==OPERAND_ADDRESS &&
            operands[0].value<=0xff &&
            operands[1].type==OPERAND_X)
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
        if (m68hc08_table[n].operand_type==CPU08_OP_OPR8_X_PLUS &&
            operands[0].type==OPERAND_ADDRESS &&
            operands[0].value<=0xff &&
            operands[1].type==OPERAND_X_PLUS)
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
        if (m68hc08_table[n].operand_type==CPU08_OP_OPR8_REL &&
            operands[0].type==OPERAND_ADDRESS &&
            operands[0].value<=0xff &&
            operands[1].type==OPERAND_ADDRESS)
        {
          if (asm_context->pass==1) { operands[1].value=asm_context->address; }
          offset=operands[1].value-(asm_context->address+3);
          if (offset<-128 || offset>127)
          {
            print_error_range("Offset", -128, 127, asm_context);
            return -1;
          }
          add_bin8(asm_context, n, IS_OPCODE);
          add_bin8(asm_context, operands[0].value, IS_OPCODE);
          add_bin8(asm_context, (char)offset, IS_OPCODE);
          return 3;
        }
          else
        if (m68hc08_table[n].operand_type==CPU08_OP_COMMA_X &&
            operands[0].type==OPERAND_NONE &&
            operands[1].type==OPERAND_X)
        {
          add_bin8(asm_context, n, IS_OPCODE);
          return 1;
        }
          else
        if (m68hc08_table[n].operand_type>=CPU08_OP_0_COMMA_OPR+operands[0].value &&
            operands[0].type==OPERAND_ADDRESS &&
            operands[0].value>=0 &&
            operands[0].value<=7 &&
            operands[1].type==OPERAND_ADDRESS &&
            operands[1].value<=0xff)
        {
          add_bin8(asm_context, n, IS_OPCODE);
          add_bin8(asm_context, operands[1].value, IS_OPCODE);
          return 2;
        }
      }
        else
      if (operand_count==3)
      {
        if (m68hc08_table[n].operand_type==CPU08_OP_X_PLUS_OPR8 &&
            operands[0].type==OPERAND_NONE &&
            operands[1].type==OPERAND_X_PLUS &&
            operands[2].type==OPERAND_ADDRESS &&
            operands[2].value<=0xff)
        {
          add_bin8(asm_context, n, IS_OPCODE);
          add_bin8(asm_context, operands[2].value, IS_OPCODE);
          return 2;
        }
          else
        if (m68hc08_table[n].operand_type==CPU08_OP_OPR8_X_PLUS_REL &&
            operands[0].type==OPERAND_ADDRESS &&
            operands[0].value<=0xff &&
            operands[1].type==OPERAND_X_PLUS &&
            operands[2].type==OPERAND_ADDRESS)
        {
          if (asm_context->pass==1) { operands[2].value=asm_context->address; }
          offset=operands[2].value-(asm_context->address+3);
          if (offset<-128 || offset>127)
          {
            print_error_range("Offset", -128, 127, asm_context);
            return -1;
          }
          add_bin8(asm_context, n, IS_OPCODE);
          add_bin8(asm_context, operands[0].value, IS_OPCODE);
          add_bin8(asm_context, (char)offset, IS_OPCODE);
          return 3;
        }
          else
        if (m68hc08_table[n].operand_type==CPU08_OP_OPR8_X_REL &&
            operands[0].type==OPERAND_ADDRESS &&
            operands[0].value<=0xff &&
            operands[1].type==OPERAND_X &&
            operands[2].type==OPERAND_ADDRESS)
        {
          if (asm_context->pass==1) { operands[2].value=asm_context->address; }
          offset=operands[2].value-(asm_context->address+3);
          if (offset<-128 || offset>127)
          {
            print_error_range("Offset", -128, 127, asm_context);
            return -1;
          }
          add_bin8(asm_context, n, IS_OPCODE);
          add_bin8(asm_context, operands[0].value, IS_OPCODE);
          add_bin8(asm_context, (char)offset, IS_OPCODE);
          return 3;
        }
          else
        if (m68hc08_table[n].operand_type==CPU08_OP_X_PLUS_REL &&
            operands[0].type==OPERAND_NONE &&
            operands[1].type==OPERAND_X_PLUS &&
            operands[2].type==OPERAND_ADDRESS)
        {
          if (asm_context->pass==1) { operands[2].value=asm_context->address; }
          offset=operands[2].value-(asm_context->address+2);
          if (offset<-128 || offset>127)
          {
            print_error_range("Offset", -128, 127, asm_context);
            return -1;
          }
          add_bin8(asm_context, n, IS_OPCODE);
          add_bin8(asm_context, (char)offset, IS_OPCODE);
          return 2;
        }
          else
        if (m68hc08_table[n].operand_type==CPU08_OP_X_REL &&
            operands[0].type==OPERAND_NONE &&
            operands[1].type==OPERAND_X &&
            operands[2].type==OPERAND_ADDRESS)
        {
          if (asm_context->pass==1) { operands[2].value=asm_context->address; }
          offset=operands[2].value-(asm_context->address+2);
          if (offset<-128 || offset>127)
          {
            print_error_range("Offset", -128, 127, asm_context);
            return -1;
          }
          add_bin8(asm_context, n, IS_OPCODE);
          add_bin8(asm_context, (char)offset, IS_OPCODE);
          return 2;
        }
          else
        if (m68hc08_table[n].operand_type>=CPU08_OP_0_COMMA_OPR_REL+operands[0].value &&
            operands[0].type==OPERAND_ADDRESS &&
            operands[0].value>=0 &&
            operands[0].value<=7 &&
            operands[1].type==OPERAND_ADDRESS &&
            operands[1].value<=0xff &&
            operands[2].type==OPERAND_ADDRESS)
        {
          if (asm_context->pass==1) { operands[2].value=asm_context->address; }
          offset=operands[2].value-(asm_context->address+3);
          if (offset<-128 || offset>127)
          {
            print_error_range("Offset", -128, 127, asm_context);
            return -1;
          }
          add_bin8(asm_context, n, IS_OPCODE);
          add_bin8(asm_context, operands[1].value, IS_OPCODE);
          add_bin8(asm_context, (char)offset, IS_OPCODE);
          return 3;
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
  while(m68hc08_16_table[n].instr!=NULL)
  {
    if (strcmp(m68hc08_16_table[n].instr, instr_case)==0)
    {
      if (asm_context->pass==2 && m68hc08_16_table[n].opcode!=opcode)
      {
        n++;
        continue;
      }

      matched=1;

      if (operand_count==2)
      {
        if (m68hc08_16_table[n].operand_type==CPU08_OP_OPR16_SP &&
            operands[0].type==OPERAND_ADDRESS &&
            operands[1].type==OPERAND_SP)
        {
          // 8 bit num can fit in here too, but better if it fits in NUM8
          add_bin8(asm_context, m68hc08_16_table[n].opcode>>8, IS_OPCODE);
          add_bin8(asm_context, m68hc08_16_table[n].opcode&0xff, IS_OPCODE);
          add_bin8(asm_context, operands[0].value>>8, IS_OPCODE);
          add_bin8(asm_context, operands[0].value&0xff, IS_OPCODE);
          size=4;
          if (asm_context->pass==2) { return size; }
        }
          else
        if (m68hc08_16_table[n].operand_type==CPU08_OP_OPR8_SP &&
            operands[0].type==OPERAND_ADDRESS &&
            operands[0].value<=0xff &&
            operands[1].type==OPERAND_SP)
        {
          if (size!=-1)
          {
            // If we previously wrote a 16 bit version, we can reverse it
            // and use a smaller, better version.
            asm_context->address-=size;
          }
          add_bin8(asm_context, m68hc08_16_table[n].opcode>>8, IS_OPCODE);
          add_bin8(asm_context, m68hc08_16_table[n].opcode&0xff, IS_OPCODE);
          add_bin8(asm_context, operands[0].value, IS_OPCODE);
          return 3;
        }
      }
        else
      if (operand_count==3)
      {
        if (m68hc08_16_table[n].operand_type==CPU08_OP_OPR8_SP_REL &&
            operands[0].type==OPERAND_ADDRESS &&
            operands[0].value<=0xff &&
            operands[1].type==OPERAND_SP &&
            operands[2].type==OPERAND_ADDRESS)
        {
          if (asm_context->pass==1)
          {
            operands[2].value=asm_context->address;
          }
          offset=operands[2].value-(asm_context->address+4);
          if (offset<-128 || offset>127)
          {
            print_error_range("Offset", -128, 127, asm_context);
            return -1;
          }
          add_bin8(asm_context, m68hc08_16_table[n].opcode>>8, IS_OPCODE);
          add_bin8(asm_context, m68hc08_16_table[n].opcode&0xff, IS_OPCODE);
          add_bin8(asm_context, operands[0].value, IS_OPCODE);
          add_bin8(asm_context, offset, IS_OPCODE);
          return 4;
        }
      }
    }
    n++;
  }

  if (size!=-1) { return size; }

  if (matched==1)
  {
    print_error_unknown_operand_combo(instr, asm_context);
    return -1;
  }

  print_error_unknown_instr(instr, asm_context);

  return -1;
}

