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
  OPERAND_H_REGISTER,
  OPERAND_NUMBER,
  OPERAND_ADDRESS,
  OPERAND_TWO_REG_IN_BRACKETS,      // [ Rb, Ro ]
  OPERAND_REG_AND_NUM_IN_BRACKETS,  // [ Rb, #IMM ]
  OPERAND_PC_AND_REG_IN_BRACKETS,   // [ PC, Rb ]
  OPERAND_PC_AND_NUM_IN_BRACKETS,   // [ PC, #IMM ]
  OPERAND_SP_AND_REG_IN_BRACKETS,   // [ SP, Rb ]
  OPERAND_SP_AND_NUM_IN_BRACKETS,   // [ SP, #IMM ]
};

struct _operand
{
  int value;
  int type;
  int second_value;
};

static int get_register_thumb(char *token)
{
  if (token[0]=='r' || token[0]=='R')
  {
    if (token[2]==0 && (token[1]>='0' && token[1]<='7'))
    {
      return token[1]-'0';
    }
  }

  return -1;
}

static int get_h_register_thumb(char *token)
{
  if (token[0]=='r' || token[0]=='R')
  {
    if (token[2]==0 && (token[1]>='8' && token[1]<='9'))
    {
      return (token[1]-'0')-8;
    }
      else
    if (token[3]==0 && token[1]=='1' && (token[2]>='0' && token[2]<='5'))
    {
      return (token[2]-'0')+10-8;
    }
  }

  if (strcasecmp("sp", token)==0) { return 13-8; }
  if (strcasecmp("lr", token)==0) { return 14-8; }
  if (strcasecmp("pc", token)==0) { return 15-8; }

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

static int is_4_byte_aligned(struct _asm_context *asm_context, int num)
{
  if ((num&0x3)!=0)
  {
    print_error("Offset not 4 byte aligned", asm_context);
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
    if ((num=get_h_register_thumb(token))!=-1)
    {
      operands[operand_count].type=OPERAND_H_REGISTER;
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

      if (strcasecmp(token,"pc")==0 || strcasecmp(token,"r15")==0)
      {
        operands[operand_count].type=OPERAND_PC_AND_REG_IN_BRACKETS;
      }
        else
      if (strcasecmp(token,"sp")==0 || strcasecmp(token,"r13")==0)
      {
        operands[operand_count].type=OPERAND_SP_AND_REG_IN_BRACKETS;
      }
        else
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
        if (operands[operand_count].type==OPERAND_PC_AND_NUM_IN_BRACKETS)
        {
          operands[operand_count].type=OPERAND_PC_AND_REG_IN_BRACKETS;
        }
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

      operands[operand_count].value=num;
      operands[operand_count].type=OPERAND_ADDRESS;
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
          if (operand_count==3 &&
              operands[0].type==OPERAND_REGISTER &&
              operands[1].type==OPERAND_REGISTER &&
              operands[2].type==OPERAND_NUMBER)
          {
            if (check_reg_lower(asm_context, operands[0].value)==-1) { return -1; }
            if (check_reg_lower(asm_context, operands[1].value)==-1) { return -1; }
            if (check_range(asm_context, "Offset", operands[2].value, 0, 31)==-1) { return -1; }
#if 0
            if (operands[2].value<0 || operands[2].value>31)
            {
              print_error_range("Offset", 0, 31, asm_context);
              return -1;
            }
#endif
            add_bin16(asm_context, table_thumb[n].opcode|(operands[2].value<<6)|(operands[1].value<<3)|(operands[0].value), IS_OPCODE);
            return 2;
          }
          break;
        case OP_ADD_SUB:
          if (operand_count==3 &&
              operands[0].type==OPERAND_REGISTER &&
              operands[1].type==OPERAND_REGISTER &&
              operands[2].type==OPERAND_REGISTER)
          {
            add_bin16(asm_context, table_thumb[n].opcode|(operands[2].value<<6)|(operands[1].value<<3)|(operands[0].value), IS_OPCODE);
            return 2;
          }
            else
          if (operand_count==3 &&
              operands[0].type==OPERAND_REGISTER &&
              operands[1].type==OPERAND_REGISTER &&
              operands[2].type==OPERAND_NUMBER)
          {
            add_bin16(asm_context, table_thumb[n].opcode|(1<<10)|(operands[2].value<<6)|(operands[1].value<<3)|(operands[0].value), IS_OPCODE);
            return 2;
          }
          break;
        case OP_IMM:
          if (operand_count==2 &&
              operands[0].type==OPERAND_REGISTER &&
              operands[1].type==OPERAND_NUMBER)
          {
            add_bin16(asm_context, table_thumb[n].opcode|(operands[0].value<<8)|(operands[1].value), IS_OPCODE);
            return 2;
          }
          break;
        case OP_ALU:
          if (operand_count==2 &&
              operands[0].type==OPERAND_REGISTER &&
              operands[1].type==OPERAND_REGISTER)
          {
            add_bin16(asm_context, table_thumb[n].opcode|(operands[1].value<<3)|(operands[0].value), IS_OPCODE);
            return 2;
          }
          break;
        case OP_HI:
          if (operand_count==2)
          {
            if (operands[0].type==OPERAND_H_REGISTER &&
                operands[1].type==OPERAND_REGISTER)
            {
              add_bin16(asm_context, table_thumb[n].opcode|(1<<7)|(operands[1].value<<3)|(operands[0].value), IS_OPCODE);
              return 2;
            }
              else
            if (operands[0].type==OPERAND_REGISTER &&
                operands[1].type==OPERAND_H_REGISTER)
            {
              add_bin16(asm_context, table_thumb[n].opcode|(1<<6)|(operands[1].value<<3)|(operands[0].value), IS_OPCODE);
              return 2;
            }
              else
            if (operands[0].type==OPERAND_H_REGISTER &&
                operands[1].type==OPERAND_H_REGISTER)
            {
              add_bin16(asm_context, table_thumb[n].opcode|(1<<7)|(1<<6)|(operands[1].value<<3)|(operands[0].value), IS_OPCODE);
              return 2;
            }
          }
          break;
        case OP_HI_BX:
          if (operand_count==1 &&
              operands[0].type==OPERAND_REGISTER)
          {
            add_bin16(asm_context, table_thumb[n].opcode|(operands[0].value<<3), IS_OPCODE);
            return 2;
          }
            else
          if (operand_count==1 &&
              operands[0].type==OPERAND_H_REGISTER)
          {
            add_bin16(asm_context, table_thumb[n].opcode|(1<<6)|(operands[0].value<<3), IS_OPCODE);
            return 2;
          }
          break;
        case OP_PC_RELATIVE_LOAD:
          // REVIEW - Docs say this is a 10 bit, 4 byte aligned number
          // and it seems unsigned.  Why isn't this signed?
          if (operand_count==2 &&
              operands[0].type==OPERAND_REGISTER &&
              operands[1].type==OPERAND_PC_AND_NUM_IN_BRACKETS)
          {
            if (check_range(asm_context, "Offset", operands[1].second_value, 0, 1020)==-1) { return -1; }
            if (is_4_byte_aligned(asm_context, operands[1].second_value)==-1) { return -1; }
#if 0
            if ((operands[1].second_value&0x3)!=0)
            {
              print_error("Offset not 4 byte aligned", asm_context);
              return -1;
            }
#endif
            add_bin16(asm_context, table_thumb[n].opcode|(operands[0].value<<8)|(operands[1].second_value>>2), IS_OPCODE);
            return 2;
          }
          if (operand_count==2 &&
              operands[0].type==OPERAND_REGISTER &&
              operands[1].type==OPERAND_ADDRESS)
          {
            // REVIEW: This looks so odd.  Docs say: The value of the PC will
            // be 4 bytes greater than the address of this instruction, but bit
            // 1 of the PC is forced to 0 to ensure it is word aligned.
            if (is_4_byte_aligned(asm_context, operands[1].value)==-1) { return -1; }
#if 0
            if ((operands[1].value&0x3)!=0)
            {
              print_error("Offset not 4 byte aligned", asm_context);
              return -1;
            }
#endif
            int offset=operands[1].value-((asm_context->address+4)&0xfffffffc);
            if (asm_context->pass==1) { offset=0; }
            if (check_range(asm_context, "Offset", offset, 0, 1020)==-1) { return -1; }
#if 0
            if (offset<0 || offset>1020)
            {
              print_error_range("Offset", 0, 1020, asm_context);
              return -1;
            }
#endif
            add_bin16(asm_context, table_thumb[n].opcode|(operands[0].value<<8)|(offset>>2), IS_OPCODE);
            return 2;
          }
          break;
        case OP_LOAD_STORE:
          if (operand_count==2 &&
              operands[0].type==OPERAND_REGISTER &&
              operands[1].type==OPERAND_TWO_REG_IN_BRACKETS)
          {
            add_bin16(asm_context, table_thumb[n].opcode|(operands[1].second_value<<6)|(operands[1].value<<3)|(operands[0].value), IS_OPCODE);
            return 2;
          }
          break;
        case OP_LOAD_STORE_SIGN_EXT_HALF_WORD:
          if (operand_count==2 &&
              operands[0].type==OPERAND_REGISTER &&
              operands[1].type==OPERAND_TWO_REG_IN_BRACKETS)
          {
            add_bin16(asm_context, table_thumb[n].opcode|(operands[1].second_value<<6)|(operands[1].value<<3)|(operands[0].value), IS_OPCODE);
            return 2;
          }
          break;
        case OP_LOAD_STORE_IMM_OFFSET_WORD:
          if (operand_count==2 &&
              operands[0].type==OPERAND_REGISTER &&
              operands[1].type==OPERAND_REG_AND_NUM_IN_BRACKETS)
          {
            int offset=operands[1].second_value;
            if (check_range(asm_context, "Offset", offset, 0, 124)==-1) { return -1; }
            if (is_4_byte_aligned(asm_context, offset)==-1) { return -1; }
#if 0
            if ((offset&0x3)!=0)
            {
              print_error("Offset not 4 byte aligned", asm_context);
              return -1;
            }
#endif
            offset=offset>>2;
            add_bin16(asm_context, table_thumb[n].opcode|(offset<<6)|(operands[1].value<<3)|(operands[0].value), IS_OPCODE);
            return 2;
          }
          break;
        case OP_LOAD_STORE_IMM_OFFSET:
          if (operand_count==2 &&
              operands[0].type==OPERAND_REGISTER &&
              operands[1].type==OPERAND_REG_AND_NUM_IN_BRACKETS)
          {
            if (check_range(asm_context, "Offset", operands[1].second_value, 0, 31)==-1) { return -1; }
            add_bin16(asm_context, table_thumb[n].opcode|(operands[1].second_value<<6)|(operands[1].value<<3)|(operands[0].value), IS_OPCODE);
            return 2;
          }
          break;
        case OP_LOAD_STORE_IMM_OFFSET_HALF_WORD:
          if (operand_count==2 &&
              operands[0].type==OPERAND_REGISTER &&
              operands[1].type==OPERAND_REG_AND_NUM_IN_BRACKETS)
          {
            int offset=operands[1].second_value;
            if (check_range(asm_context, "Offset", offset, 0, 60)==-1) { return -1; }
            if ((offset&0x1)!=0)
            {
              print_error("Offset not 2 byte aligned", asm_context);
              return -1;
            }
            offset=offset>>1;
            add_bin16(asm_context, table_thumb[n].opcode|(offset<<6)|(operands[1].value<<3)|(operands[0].value), IS_OPCODE);
            return 2;
          }
          break;
        case OP_LOAD_STORE_SP_RELATIVE:
          if (operand_count==2 &&
              operands[0].type==OPERAND_REGISTER &&
              operands[1].type==OPERAND_SP_AND_NUM_IN_BRACKETS)
          {
            if (check_range(asm_context, "Offset", operands[1].value, 0, 1020)==-1) { return -1; }
            if (is_4_byte_aligned(asm_context, operands[1].value)==-1) { return -1; }
            add_bin16(asm_context, table_thumb[n].opcode|(operands[0].value<<8)|(operands[1].second_value>>2), IS_OPCODE);
            return 2;
          }
          break;
        case OP_LOAD_ADDRESS:
          if (operand_count==3 &&
              operands[0].type==OPERAND_REGISTER &&
              operands[1].type==OPERAND_H_REGISTER &&
              operands[2].type==OPERAND_NUMBER)
          {
            if (check_range(asm_context, "Offset", operands[2].value, 0, 1020)==-1) { return -1; }
            if (is_4_byte_aligned(asm_context, operands[2].value)==-1) { return -1; }
            if (operands[1].value==7)
            {
              add_bin16(asm_context, table_thumb[n].opcode|(operands[0].value<<8)|(operands[2].value>>2), IS_OPCODE);
              return 2;
            }
              else
            if (operands[1].value==5)
            {
              add_bin16(asm_context, table_thumb[n].opcode|(1<<11)|(operands[0].value<<8)|(operands[2].value>>2), IS_OPCODE);
              return 2;
            }
          }
          break;
        case OP_ADD_OFFSET_TO_SP:
          if (operand_count==2 &&
              operands[0].type==OPERAND_H_REGISTER &&
              operands[0].value==5 &&
              operands[1].type==OPERAND_NUMBER)
          {
            // FIXME - According to the docs, what I did here is wrong.
            // it says that the offset is +-508 and i have +-1020.
            if (check_range(asm_context, "Offset", operands[1].value, -1020, 1020)==-1) { return -1; }
            int s=0;
            if (operands[1].value<0)
            {
              operands[1].value=-operands[1].value;
              s=1;
            }
            if (is_4_byte_aligned(asm_context, operands[1].value)==-1) { return -1; }
            add_bin16(asm_context, table_thumb[n].opcode|(s<<8)|(operands[1].value>>2), IS_OPCODE);
            return 2;
          }
          break;
        default:
          break;
      }
    }

    n++;
  }

  if (matched==1)
  {
    print_error_unknown_operand_combo(instr, asm_context);
  }
    else
  {
    print_error_unknown_instr(instr, asm_context);
  }

  return -1;
}



