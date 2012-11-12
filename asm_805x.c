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

#include "asm_805x.h"
#include "asm_common.h"
#include "assembler.h"
#include "disasm_arm.h"
#include "get_tokens.h"
#include "eval_expression.h"
#include "table_805x.h"

enum
{
  OPERAND_REG,
  OPERAND_AT_REG,
  OPERAND_A,
  OPERAND_C,
  OPERAND_AB,
  OPERAND_DPTR,
  OPERAND_AT_A_PLUS_DPTR,
  OPERAND_AT_A_PLUS_PC,
  OPERAND_AT_DPTR,
  OPERAND_DATA,
  OPERAND_NUM,
  OPERAND_SLASH_NUM,
  OPERAND_BIT_ADDRESS,
};

struct _operand
{
  int value;
  int type;
};

static int get_register_805x(char *token)
{
  if (token[0]!='r' && token[0]!='R') return -1;
  if (token[1]>='0' && token[1]<='7' && token[2]==0)
  {
    return token[1]-'0';
  }

  return -1;
}

int parse_instruction_805x(struct _asm_context *asm_context, char *instr)
{
char instr_lower_mem[TOKENLEN];
char *instr_lower=instr_lower_mem;
char token[TOKENLEN];
struct _operand operands[3];
int operand_count=0;
int token_type;
int matched=0;
int num,n,r;
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

    num=get_register_805x(token);
    if (num!=-1)
    {
      operands[operand_count].type=OPERAND_REG;
      operands[operand_count].value=num;
    }
      else
    if (IS_TOKEN(token,'A'))
    {
      operands[operand_count].type=OPERAND_A;
    }
      else
    if (IS_TOKEN(token,'C'))
    {
      operands[operand_count].type=OPERAND_C;
    }
      else
    if (strcasecmp(token, "ab")==0)
    {
      operands[operand_count].type=OPERAND_AB;
    }
      else
    if (strcasecmp(token, "dptr")==0)
    {
      operands[operand_count].type=OPERAND_DPTR;
    }
      else
    if (IS_TOKEN(token,'@'))
    {
      token_type=get_token(asm_context, token, TOKENLEN);
      num=get_register_805x(token);
      if (num!=-1)
      {
        operands[operand_count].type=OPERAND_AT_REG;
        operands[operand_count].value=num;
      }
        else
      if (strcasecmp(token, "dptr")==0)
      {
        operands[operand_count].type=OPERAND_AT_DPTR;
      }
        else
      if (strcasecmp(token, "a")==0)
      {
        do
        {
          token_type=get_token(asm_context, token, TOKENLEN);
          if (IS_NOT_TOKEN(token,'+')) break;
          token_type=get_token(asm_context, token, TOKENLEN);
          if (strcasecmp(token, "dptr")==0)
          {
            operands[operand_count].type=OPERAND_AT_A_PLUS_DPTR;
          }
            else
          if (strcasecmp(token, "pc")==0)
          {
            operands[operand_count].type=OPERAND_AT_A_PLUS_PC;
          }
        } while(0);

        if (operands[operand_count].type==0);
        {
          print_error_unexp(token, asm_context);
          return -1;
        }
      }
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
    if (IS_TOKEN(token,'/'))
    {
      token_type=get_token(asm_context, token, TOKENLEN);
      if (token_type!=TOKEN_NUMBER)
      {
        print_error_unexp(token, asm_context);
        return -1;
      }
      operands[operand_count].type=OPERAND_SLASH_NUM;
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
for (n=0; n<operand_count; n++)
{
printf("[%d %d]", operands[n].type, operands[n].value);
}
printf("\n");
#endif

  for (n=0; n<256; n++)
  {
    if (strcmp(table_805x[n].name, instr_lower)==0)
    {
      matched=1;
      for(r=0; r<3; r++)
      {
        if (table_805x[n].op[r]==OP_NONE) { break; }

        switch(table_805x[n].op[r])
        {
          case OP_REG:
            if (operands[r].type!=OPERAND_REG ||
                operands[r].value!=table_805x[n].range) { r=4; }
            break;
          case OP_AT_REG:
            if (operands[r].type!=OPERAND_AT_REG ||
                operands[r].value!=table_805x[n].range) { r=4; }
            break;
          case OP_A:
            if (operands[r].type!=OPERAND_A) { r=4; }
            break;
          case OP_C:
            if (operands[r].type!=OPERAND_C) { r=4; }
            break;
          case OP_AB:
            if (operands[r].type!=OPERAND_AB) { r=4; }
            break;
          case OP_DPTR:
            if (operands[r].type!=OPERAND_DPTR) { r=4; }
            break;
          case OP_AT_A_PLUS_DPTR:
            if (operands[r].type!=OPERAND_AT_A_PLUS_DPTR) { r=4; }
            break;
          case OP_AT_A_PLUS_PC:
            if (operands[r].type!=OPERAND_AT_A_PLUS_PC) { r=4; }
            break;
          case OP_AT_DPTR:
            if (operands[r].type!=OPERAND_AT_DPTR) { r=4; }
            break;
          case OP_DATA_16:
            if (operands[r].type!=OPERAND_DATA ||
                (operands[r].value<-32768 || 
                 operands[r].value>32767)) { r=4; }
            break;
          case OP_CODE_ADDR:
            if (operands[r].type!=OPERAND_NUM ||
                (operands[r].value<-32768 || 
                 operands[r].value>32767)) { r=4; }
            break;
          case OP_RELADDR:
            if (operands[r].type!=OPERAND_NUM) { r=4; }
            break;
          case OP_DATA:
            if (operands[r].type!=OPERAND_DATA ||
                (operands[r].value<-128 || 
                 operands[r].value>255)) { r=4; }
            break;
          case OP_SLASH_BIT_ADDR:
            if (operands[r].type!=OPERAND_SLASH_NUM||
                (operands[r].value<-128 || 
                 operands[r].value>255)) { r=4; }
            break;
          case OP_PAGE:
            if ((operands[r].value>>8)!=table_805x[n].range) { r=4; break; }
            break;
          case OP_BIT_ADDR:
          case OP_IRAM_ADDR:
            if (operands[r].type!=OPERAND_NUM||
                (operands[r].value<-128 || 
                 operands[r].value>255)) { r=4; }
            break;
          default:
            printf("Internal error %s:%d\n", __FILE__, __LINE__);
            return -1;
        }
      }

      if (r==operand_count)
      {
        memory_write_inc(asm_context, n, asm_context->line);
        for(r=0; r<3; r++)
        {
          if (table_805x[n].op[r]==OP_NONE) { break; }
          switch(table_805x[n].op[r])
          {
            case OP_DATA_16:
            case OP_CODE_ADDR:
            {
              unsigned short int value=operands[r].value&0xffff;
              memory_write_inc(asm_context, value&0xff, asm_context->line);
              memory_write_inc(asm_context, value>>8, asm_context->line);
              count+=2;
              break;
            }
            case OP_RELADDR:
            {
              num=operands[r].value-(asm_context->address+1);
              memory_write_inc(asm_context, (unsigned char)num, asm_context->line);
              count++;
              break;
            }
            case OP_DATA:
            case OP_SLASH_BIT_ADDR:
            case OP_PAGE:
            case OP_BIT_ADDR:
            case OP_IRAM_ADDR:
            {
              memory_write_inc(asm_context, (unsigned char)operands[r].value&0xff, asm_context->line);
              count++;
              break;
            }
          }
        }

        break;
      }
    }
  }

  if (n==256)
  {
    if (matched==1)
    {
      printf("Error: Unknown operands combo for '%s' at %s:%d.\n", instr, asm_context->filename, asm_context->line);
    }
      else
    {
      print_error_unknown_instr(instr, asm_context);
    }
  }

  return count; 
}


