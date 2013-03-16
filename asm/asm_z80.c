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
#include "asm_z80.h"
#include "assembler.h"
#include "table_z80.h"
#include "get_tokens.h"
#include "eval_expression.h"

enum
{
  OPERAND_NONE,
  OPERAND_NUMBER,
  OPERAND_REG8,
  OPERAND_REG_IHALF,
  OPERAND_INDEX_REG16_XY,
  OPERAND_INDEX_REG16,
  OPERAND_INDEX_REG8,
  OPERAND_REG16_XY,
  OPERAND_REG_SPECIAL,
  OPERAND_REG16,
  OPERAND_COND,
  OPERAND_INDEX_NUMBER,
};

enum
{
  REG_B=0,
  REG_C,
  REG_D,
  REG_E,
  REG_H,
  REG_L,
  REG_INDEX_HL,  // hmmmm?
  REG_A=7,
};

enum
{
  REG_IXH=0,
  REG_IXL,
  REG_IYH,
  REG_IYL,
};

enum
{
  REG_IX=0,
  REG_IY,
};

enum
{
  REG_BC=0,
  REG_DE,
  REG_HL,
  REG_SP,
};

enum
{
  REG_AF,
  REG_AF_TICK,
  REG_F,
};

enum
{
  COND_NZ,
  COND_Z,
  COND_NC,
  COND_C,
  COND_PO,
  COND_PE,
  COND_P,
  COND_M,
};

struct _operand
{
  int value;
  int type;
  int offset;
};

int get_cond(char *token)
{
char *cond[] = { "nz","z","nc","c", "po","pe","p","m" };
int n;

  for (n=0; n<8; n++)
  {
    if (strcasecmp(token, cond[n])==0) { return n; }
  }

  return -1;
}

int get_reg8(char *token)
{
char *reg8[] = { "b","c","d","e","h","l","(hl)","a" };
int n;

  for (n=0; n<8; n++)
  {
    if (strcasecmp(token, reg8[n])==0) { return n; }
  }

  return -1;
}

int get_reg_ihalf(char *token)
{
char *reg_ihalf[] = { "ixh","ixl","iyh","iyl" };
int n;

  for (n=0; n<4; n++)
  {
    if (strcasecmp(token, reg_ihalf[n])==0) { return n; }
  }

  return -1;
}

int get_reg_index(char *token)
{
char *reg_index[] = { "ix","iy" };
int n;

  for (n=0; n<2; n++)
  {
    if (strcasecmp(token, reg_index[n])==0) { return n; }
  }

  return -1;
}

int get_reg16(char *token)
{
char *reg16[] = { "bc","de","hl","sp" };
int n;

  for (n=0; n<4; n++)
  {
    if (strcasecmp(token, reg16[n])==0) { return n; }
  }

  return -1;
}

int get_reg_special(char *token)
{
char *reg_special[] = { "af","af'","f" };
int n;

  for (n=0; n<3; n++)
  {
    if (strcasecmp(token, reg_special[n])==0) { return n; }
  }

  return -1;
}

int parse_instruction_z80(struct _asm_context *asm_context, char *instr)
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

    if (IS_TOKEN(token,'('))
    {
      token_type=get_token(asm_context, token, TOKENLEN);
      if ((n=get_reg16(token))!=-1)
      {
        operands[operand_count].type=OPERAND_INDEX_REG16;
        operands[operand_count].value=n;
      }
        else
      if ((n=get_reg8(token))!=-1)
      {
        operands[operand_count].type=OPERAND_INDEX_REG8;
        operands[operand_count].value=n;
      }
        else
      if ((n=get_reg_index(token))!=-1)
      {
        operands[operand_count].type=OPERAND_INDEX_REG16_XY;
        operands[operand_count].value=n;
        token_type=get_token(asm_context, token, TOKENLEN);
        pushback(asm_context, token, token_type);
        if (IS_NOT_TOKEN(token,')'))
        {
          if (eval_expression(asm_context, &num)!=0)
          {
            if (asm_context->pass==1)
            {
              eat_operand(asm_context);
              num=0;
            }
              else
            {
              print_error_illegal_expression(instr, asm_context);
              return -1;
            }
          }
          operands[operand_count].offset=num;
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
            num=0;
          }
            else
          {
            print_error_illegal_expression(instr, asm_context);
            return -1;
          }
        }

        operands[operand_count].type=OPERAND_INDEX_NUMBER;
        operands[operand_count].value=num;

        //print_error_unexp(token, asm_context);
        //return -1;
      }
      if (expect_token_s(asm_context,")")!=0) { return -1; }
    }
      else
    if ((n=get_reg8(token))!=-1)
    {
      operands[operand_count].type=OPERAND_REG8;
      operands[operand_count].value=n;
    }
      else
    if ((n=get_reg_ihalf(token))!=-1)
    {
      operands[operand_count].type=OPERAND_REG_IHALF;
      operands[operand_count].value=n;
    }
      else
    if ((n=get_reg16(token))!=-1)
    {
      operands[operand_count].type=OPERAND_REG16;
      operands[operand_count].value=n;
    }
      else
    if ((n=get_reg_index(token))!=-1)
    {
      operands[operand_count].type=OPERAND_REG16_XY;
      operands[operand_count].value=n;
    }
      else
    if ((n=get_reg_special(token))!=-1)
    {
      operands[operand_count].type=OPERAND_REG_SPECIAL;
      operands[operand_count].value=n;
    }
      else
    if ((n=get_cond(token))!=-1)
    {
      operands[operand_count].type=OPERAND_COND;
      operands[operand_count].value=n;
    }
      else
    {
      pushback(asm_context, token, token_type);

      if (eval_expression(asm_context, &num)!=0)
      {
        if (asm_context->pass==1)
        {
          eat_operand(asm_context);
          num=0;
        }
          else
        {
          print_error_illegal_expression(instr, asm_context);
          return -1;
        }
      }

      operands[operand_count].type=OPERAND_NUMBER;
      operands[operand_count].value=num;

      if (num<-32768 || num>65535)
      {
        print_error_range("Constant", -32768, 65535, asm_context);
        return -1;
      }
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

#if DEBUG
printf("operand_count=%d\n", operand_count);
for (n=0; n<operand_count; n++)
{
printf("-- %d %d %d\n", operands[n].type, operands[n].value, operands[n].offset);
}
#endif

  // Instruction is parsed, now find matching opcode

  n=0;
  while(table_z80[n].instr!=NULL)
  {
    if (strcmp(table_z80[n].instr,instr_case)==0)
    {
       matched=1;
      switch(table_z80[n].type)
      {
        case OP_NONE:
          if (operand_count==0)
          {
            add_bin8(asm_context, table_z80[n].opcode, IS_OPCODE);
            return 1;
          }
        case OP_NONE16:
          if (operand_count==0)
          {
            add_bin8(asm_context, table_z80[n].opcode>>8, IS_OPCODE);
            add_bin8(asm_context, table_z80[n].opcode&0xff, IS_OPCODE);
            return 2;
          }
        case OP_NONE24:
          if (operand_count==0)
          {
            add_bin8(asm_context, table_z80[n].opcode>>8, IS_OPCODE);
            add_bin8(asm_context, table_z80[n].opcode&0xff, IS_OPCODE);
            add_bin8(asm_context, 0, IS_OPCODE);
            return 3;
          }
        case OP_A_REG8:
          if (operand_count==2 &&
              operands[0].type==OPERAND_REG8 &&
              operands[0].value==REG_A &&
              operands[1].type==OPERAND_REG8)
          {
            add_bin8(asm_context, table_z80[n].opcode|operands[1].value, IS_OPCODE);
            return 1;
          }
          break;
        case OP_REG8:
          if (operand_count==1 &&
              operands[0].type==OPERAND_REG8)
          {
            add_bin8(asm_context, table_z80[n].opcode|operands[0].value, IS_OPCODE);
            return 1;
          }
          break;
        case OP_A_REG_IHALF:
          if (operand_count==2 &&
              operands[0].type==OPERAND_REG8 &&
              operands[0].value==REG_A &&
              operands[1].type==OPERAND_REG_IHALF)
          {
            unsigned char y=(operands[1].value>>1);
            unsigned char l=(operands[1].value&0x1);
            add_bin8(asm_context, (table_z80[n].opcode>>8)|(y<<5), IS_OPCODE);
            add_bin8(asm_context, (table_z80[n].opcode&0xff)|l, IS_OPCODE);
            return 2;
          }
          break;
        case OP_A_INDEX:
          if (operand_count==2 &&
              operands[0].type==OPERAND_REG8 &&
              operands[0].value==REG_A &&
              operands[1].type==OPERAND_INDEX_REG16_XY)
          {
            add_bin8(asm_context, (table_z80[n].opcode>>8)|((operands[1].value&0x1)<<5), IS_OPCODE);
            add_bin8(asm_context, table_z80[n].opcode&0xff, IS_OPCODE);
            add_bin8(asm_context, (unsigned char)operands[1].offset, IS_OPCODE);
            return 3;
          }
          break;
        case OP_A_NUMBER8:
          if (operand_count==2 &&
              operands[0].type==OPERAND_REG8 &&
              operands[0].value==REG_A &&
              operands[1].type==OPERAND_NUMBER)
          {
            if (operands[1].value<-128 || operands[1].value>127)
            {
              print_error_range("Constant", -128, 127, asm_context);
              return -1;
            }
            add_bin8(asm_context, table_z80[n].opcode, IS_OPCODE);
            add_bin8(asm_context, (unsigned char)operands[1].value, IS_OPCODE);
            return 2;
          }
          break;
        case OP_HL_REG16_1:
          if (operand_count==2 &&
              operands[0].type==OPERAND_REG16 &&
              operands[0].value==REG_HL &&
              operands[1].type==OPERAND_REG16)
          {
            add_bin8(asm_context, table_z80[n].opcode|(operands[1].value<<4), IS_OPCODE);
            return 1;
          }
          break;
        case OP_HL_REG16_2:
          if (operand_count==2 &&
              operands[0].type==OPERAND_REG16 &&
              operands[0].value==REG_HL &&
              operands[1].type==OPERAND_REG16)
          {
            add_bin8(asm_context, table_z80[n].opcode>>8, IS_OPCODE);
            add_bin8(asm_context, (table_z80[n].opcode&0xff)|(operands[1].value<<4), IS_OPCODE);
            return 2;
          }
          break;
        case OP_XY_REG16:
          if (operands[0].type==OPERAND_REG16_XY &&
              operands[0].type==operands[1].type &&
              operands[0].value==operands[1].value)
          {
            operands[1].type=OPERAND_REG16;
            operands[1].value=REG_HL;
          }
          if (operand_count==2 &&
              operands[0].type==OPERAND_REG16_XY &&
              operands[1].type==OPERAND_REG16)
          {
            operands[0].value&=0x1;
            add_bin8(asm_context, (table_z80[n].opcode>>8)|(operands[0].value<<5), IS_OPCODE);
            add_bin8(asm_context, (table_z80[n].opcode&0xff)|(operands[1].value<<4), IS_OPCODE);
            return 2;
          }
          break;
        case OP_A_INDEX_HL:
          if (operand_count==2 &&
              operands[0].type==OPERAND_REG8 &&
              operands[0].value==REG_A &&
              operands[1].type==OPERAND_INDEX_REG16 &&
              operands[1].value==REG_HL)
          {
            add_bin8(asm_context, table_z80[n].opcode, IS_OPCODE);
            return 1;
          }
          break;
        case OP_INDEX_HL:
          if (operand_count==1 &&
              operands[0].type==OPERAND_INDEX_REG16 &&
              operands[0].value==REG_HL)
          {
            add_bin8(asm_context, table_z80[n].opcode, IS_OPCODE);
            return 1;
          }
          break;
        case OP_NUMBER8:
          if (operand_count==1 &&
              operands[0].type==OPERAND_NUMBER)
          {
            if (operands[0].value<-128 || operands[1].value>127)
            {
              print_error_range("Constant", -128, 127, asm_context);
              return -1;
            }
            add_bin8(asm_context, table_z80[n].opcode, IS_OPCODE);
            add_bin8(asm_context, (unsigned char)operands[0].value, IS_OPCODE);
            return 2;
          }
          break;
        case OP_REG_IHALF:
          if (operand_count==1 &&
              operands[0].type==OPERAND_REG_IHALF)
          {
            unsigned char y=(operands[0].value>>1);
            unsigned char l=(operands[0].value&0x1);
            add_bin8(asm_context, (table_z80[n].opcode>>8)|(y<<5), IS_OPCODE);
            add_bin8(asm_context, (table_z80[n].opcode&0xff)|l, IS_OPCODE);
            return 2;
          }
          break;
        case OP_INDEX:
          if (operand_count==1 &&
              operands[0].type==OPERAND_INDEX_REG16_XY)
          {
            add_bin8(asm_context, (table_z80[n].opcode>>8)|((operands[0].value&0x1)<<5), IS_OPCODE);
            add_bin8(asm_context, table_z80[n].opcode&0xff, IS_OPCODE);
            add_bin8(asm_context, (unsigned char)operands[0].offset, IS_OPCODE);
            return 3;
          }
          break;
        case OP_BIT_REG8:
          if (operand_count==2 &&
              operands[0].type==OPERAND_NUMBER &&
              operands[1].type==OPERAND_REG8)
          {
            add_bin8(asm_context, table_z80[n].opcode>>8, IS_OPCODE);
            add_bin8(asm_context, (table_z80[n].opcode&0xff)|(operands[0].value<<3)|operands[1].value, IS_OPCODE);
            return 2;
          }
          break;
        case OP_BIT_INDEX_HL:
          if (operand_count==2 &&
              operands[0].type==OPERAND_NUMBER &&
              operands[1].type==OPERAND_INDEX_REG16 &&
              operands[1].value==REG_HL)
          {
            add_bin8(asm_context, table_z80[n].opcode>>8, IS_OPCODE);
            add_bin8(asm_context, (table_z80[n].opcode&0xff)|(operands[0].value<<3), IS_OPCODE);
            return 2;
          }
          break;
        case OP_BIT_INDEX:
          if (operand_count==2 &&
              operands[0].type==OPERAND_NUMBER &&
              operands[1].type==OPERAND_INDEX_REG16_XY)
          {
            add_bin8(asm_context, (table_z80[n].opcode>>8)|((operands[1].value&0x1)<<5), IS_OPCODE);
            add_bin8(asm_context, table_z80[n].opcode&0xff, IS_OPCODE);
            add_bin8(asm_context, (unsigned char)operands[1].offset, IS_OPCODE);
            add_bin8(asm_context, 0x40|(operands[0].value<<3), IS_OPCODE);
            return 4;
          }
          break;
        case OP_ADDRESS:
          if (operand_count==1 &&
              operands[0].type==OPERAND_NUMBER)
          {
            add_bin8(asm_context, table_z80[n].opcode, IS_OPCODE);
            add_bin8(asm_context, operands[0].value&0xff, IS_OPCODE);
            add_bin8(asm_context, (unsigned char)(operands[0].value>>8), IS_OPCODE);
            return 3;
          }
          break;
        case OP_COND_ADDRESS:
          if (operands[0].type==OPERAND_REG8 &&
              operands[0].value==REG_C)
          {
            operands[0].type=OPERAND_COND;
            operands[0].value=COND_C;
          }
          if (operand_count==2 &&
              operands[0].type==OPERAND_COND &&
              operands[1].type==OPERAND_NUMBER)
          {
            add_bin8(asm_context, table_z80[n].opcode|(operands[0].value<<3), IS_OPCODE);
            add_bin8(asm_context, operands[1].value&0xff, IS_OPCODE);
            add_bin8(asm_context, (unsigned char)(operands[1].value>>8), IS_OPCODE);
            return 3;
          }
          break;
        case OP_REG8_V2:
          if (operand_count==1 && operands[0].type==OPERAND_REG8)
          {
            add_bin8(asm_context, table_z80[n].opcode|(operands[0].value<<3), IS_OPCODE);
            return 1;
          }
          break;
        case OP_REG_IHALF_V2:
          if (operand_count==1 &&
              operands[0].type==OPERAND_REG_IHALF)
          {
            unsigned char y=(operands[0].value>>1);
            unsigned char l=(operands[0].value&0x1);
            add_bin8(asm_context, (table_z80[n].opcode>>8)|(y<<5), IS_OPCODE);
            add_bin8(asm_context, (table_z80[n].opcode&0xff)|(l<<3), IS_OPCODE);
            return 2;
          }
          break;
        case OP_REG16:
          if (operand_count==1 &&
              operands[0].type==OPERAND_REG16)
          {
            add_bin8(asm_context, table_z80[n].opcode|(operands[0].value<<4), IS_OPCODE);
            return 1;
          }
          break;
        case OP_XY:
          if (operand_count==1 &&
              operands[0].type==OPERAND_REG16_XY)
          {
            add_bin8(asm_context, (table_z80[n].opcode>>8)|(operands[0].value<<5), IS_OPCODE);
            add_bin8(asm_context, table_z80[n].opcode&0xff, IS_OPCODE);
            return 2;
          }
          break;
        case OP_INDEX_SP_HL:
          if (operand_count==2 &&
              operands[0].type==OPERAND_INDEX_REG16 &&
              operands[0].value==REG_SP &&
              operands[1].type==OPERAND_REG16 &&
              operands[1].value==REG_HL)
          {
            add_bin8(asm_context, table_z80[n].opcode, IS_OPCODE);
            return 1;
          }
          break;
        case OP_INDEX_SP_XY:
          if (operand_count==2 &&
              operands[0].type==OPERAND_INDEX_REG16 &&
              operands[0].value==REG_SP &&
              operands[1].type==OPERAND_REG16_XY)
          {
            add_bin8(asm_context, (table_z80[n].opcode>>8)|(operands[1].value<<13), IS_OPCODE);
            add_bin8(asm_context, table_z80[n].opcode&0xff, IS_OPCODE);
            return 2;
          }
          break;
        case OP_AF_AF_TICK:
          if (operand_count==2 &&
              operands[0].type==OPERAND_REG_SPECIAL &&
              operands[0].value==REG_AF &&
              operands[1].type==OPERAND_REG_SPECIAL &&
              operands[1].value==REG_AF_TICK)
          {
            add_bin8(asm_context, table_z80[n].opcode, IS_OPCODE);
            return 1;
          }
          break;
        case OP_DE_HL:
          if (operand_count==2 &&
              operands[0].type==OPERAND_REG16 &&
              operands[0].value==REG_DE &&
              operands[1].type==OPERAND_REG16 &&
              operands[1].value==REG_HL)
          {
            add_bin8(asm_context, table_z80[n].opcode, IS_OPCODE);
            return 1;
          }
          break;
        case OP_IM_NUM:
          if (operand_count==1 &&
              operands[0].type==OPERAND_NUMBER)
          {
            if (operands[0].value<0 || operands[0].value>2)
            {
              print_error_range("Constant", 0, 2, asm_context);
              return -1;
            }
            if (operands[0].value!=0) { operands[0].value++; }

            add_bin8(asm_context, table_z80[n].opcode>>8, IS_OPCODE);
            add_bin8(asm_context, (table_z80[n].opcode&0xff)|(operands[0].value<<3), IS_OPCODE);
            return 2;
          }
          break;
        case OP_A_INDEX_N:
          if (operand_count==2 &&
              operands[0].type==OPERAND_REG8 &&
              operands[0].value==REG_A &&
              operands[1].type==OPERAND_INDEX_NUMBER)
          {
            if (operands[0].value<0 || operands[0].value>255)
            {
              print_error_range("Constant", 0, 255, asm_context);
              return -1;
            }
            add_bin8(asm_context, table_z80[n].opcode, IS_OPCODE);
            add_bin8(asm_context, operands[1].value, IS_OPCODE);
            return 2;
          }
          break;
        case OP_REG8_INDEX_C:
          if (operand_count==2 &&
              operands[0].type==OPERAND_REG8 &&
              operands[1].type==OPERAND_INDEX_REG8 &&
              operands[1].value==REG_C)
          {
            add_bin8(asm_context, table_z80[n].opcode>>8, IS_OPCODE);
            add_bin8(asm_context, (table_z80[n].opcode&0xff)|(operands[0].value<<3), IS_OPCODE);
            return 2;
          }
          break;
        case OP_F_INDEX_C:
          if (operand_count==2 &&
              operands[0].type==OPERAND_REG_SPECIAL &&
              operands[0].value==REG_F &&
              operands[1].type==OPERAND_INDEX_REG8 &&
              operands[1].value==REG_C)
          {
            add_bin8(asm_context, table_z80[n].opcode>>8, IS_OPCODE);
            add_bin8(asm_context, (table_z80[n].opcode&0xff), IS_OPCODE);
            return 2;
          }
          break;
        case OP_INDEX_XY:
          if (operand_count==1 &&
              operands[0].type==OPERAND_INDEX_REG16_XY)
          {
            add_bin8(asm_context, (table_z80[n].opcode>>8)|((operands[0].value)<<5), IS_OPCODE);
            add_bin8(asm_context, table_z80[n].opcode&0xff, IS_OPCODE);
            return 2;
          }
          break;
        case OP_JR_COND_ADDRESS:
          if (operands[0].type==OPERAND_REG8 &&
              operands[0].value==REG_C)
          {
            operands[0].type=OPERAND_COND;
            operands[0].value=COND_C;
          }
          if (operand_count==2 &&
              operands[0].type==OPERAND_COND &&
              operands[0].value<4 &&
              operands[1].type==OPERAND_NUMBER)
          {
            if (operands[1].value<0 || operands[1].value>255)
            {
              print_error_range("Address", 0, 255, asm_context);
              return -1;
            }
            add_bin8(asm_context, table_z80[n].opcode|((operands[0].value)<<3), IS_OPCODE);
            add_bin8(asm_context, operands[1].value, IS_OPCODE);
            return 2;
          }
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



