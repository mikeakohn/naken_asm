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

#include "asm_common.h"
#include "asm_mips.h"
#include "assembler.h"
#include "disasm_mips.h"
#include "get_tokens.h"
#include "eval_expression.h"

enum
{
  OPERAND_TREG,
  OPERAND_FREG,
  OPERAND_IMMEDIATE,
  OPERAND_IMMEDIATE_RS,
};

struct _operand
{
  int value;
  int type;
  int reg2;
};

static int get_register_mips(char *token, char letter)
{
  if (token[0]!='$') { return -1; }
  if (token[1]!=letter) { return -1; }

  if (token[3]==0 && (token[2]>='0' && token[2]<='9'))
  {
    return token[2]-'0';
  }
    else
  if (token[4]==0 && token[2]=='1' && (token[3]>='0' && token[3]<='5'))
  {
    return 10+(token[3]-'0');
  }

  return -1;
}

int parse_instruction_mips(struct _asm_context *asm_context, char *instr)
{
struct _operand operands[3];
int operand_count=0;
char token[TOKENLEN];
int token_type;
char instr_case[TOKENLEN];
int num,n,r;
int opcode;
#if 0
int n,cond,s=0;
int opcode=0;
#endif

  lower_copy(instr_case, instr);

//printf("%s\n", instr_case);

  while(1)
  {
    token_type=get_token(asm_context, token, TOKENLEN);
    if (token_type==TOKEN_EOL) { break; }
    //printf("token=%s token_type=%d\n", token, token_type);

    if (operand_count==0 && IS_TOKEN(token,'.'))
    {
      strcat(instr_case, ".");
      strcat(instr, ".");
      token_type=get_token(asm_context, token, TOKENLEN);
      strcat(instr, token);
      n=0;
      while(token[n]!=0) { token[n]=tolower(token[n]); n++; }
      strcat(instr_case, token);
      continue;
    }

    do
    {
      num=get_register_mips(token, 't');
      if (num!=-1)
      {
        operands[operand_count].value=num;
        operands[operand_count].type=OPERAND_TREG;
        break;
      }

      num=get_register_mips(token, 'f');
      if (num!=-1)
      {
        operands[operand_count].value=num;
        operands[operand_count].type=OPERAND_FREG;
        break;
      }

      operands[operand_count].type=OPERAND_IMMEDIATE;

      if (asm_context->pass==1)
      {
        eat_operand(asm_context);
        break;
      }

      pushback(asm_context, token, token_type);
      if (eval_expression(asm_context, &num)!=0)
      {
        print_error_unexp(token, asm_context);
        return -1;
      }

      operands[operand_count].value=num;

      token_type=get_token(asm_context, token, TOKENLEN);
      if (IS_TOKEN(token,'('))
      {
        token_type=get_token(asm_context, token, TOKENLEN);
        num=get_register_mips(token, 't');
        if (num==-1)
        {
          print_error_unexp(token, asm_context);
          return -1;
        }

        operands[operand_count].reg2=num;
        operands[operand_count].type=OPERAND_IMMEDIATE_RS;;

        token_type=get_token(asm_context, token, TOKENLEN);

        if (IS_NOT_TOKEN(token,')'))
        {
          print_error_unexp(token, asm_context);
          return -1;
        }
      }
        else
      {
        pushback(asm_context, token, token_type);
      }

      break;
    } while(1);

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

  if (asm_context->pass==1)
  {
    add_bin32(asm_context, 0, IS_OPCODE);
    return 4;
  }

  // R-Type Instruction [ op 6, rs 5, rt 5, rd 5, sa 5, function 6 ]
  n=0;
  while(mips_r_table[n].instr!=NULL)
  {
    if (strcmp(instr_case, mips_r_table[n].instr)==0)
    {
      char shift_table[] = { 0, 11, 21, 16, 6 };
      if (mips_r_table[n].operand_count!=operand_count)
      {
        printf("Error: Wrong number of operands for '%s' at %s:%d\n", instr, asm_context->filename, asm_context->line);
        return -1;
      }

      opcode=mips_r_table[n].function;

      for (r=0; r<operand_count; r++)
      {
        if (operands[r].type!=OPERAND_TREG)
        {
          printf("Error: '%s' expects registers at %s:%d\n", instr, asm_context->filename, asm_context->line);
          return -1;
        }
printf("%s  %d<<%d\n", instr, operands[r].value, shift_table[(int)mips_r_table[n].operand[r]]);
        opcode|=operands[r].value<<shift_table[(int)mips_r_table[n].operand[r]];
      }

      add_bin32(asm_context, opcode, IS_OPCODE);
      return 4;
    }
    n++;
  }

  // J-Type Instruction [ op 6, target 26 ]
  if (strcmp(instr_case, "ja")==0 || strcmp(instr_case, "jal")==0)
  {
    // FIXME - what to do with this
    //unsigned int upper=(address+4)&0xf0000000;
    if (operand_count!=1)
    {
      printf("Error: Wrong number of operands for '%s' at %s:%d\n", instr, asm_context->filename, asm_context->line);
      return -1;
    }

    if (operands[0].type!=OPERAND_IMMEDIATE)
    {
      printf("Error: Expecting address for '%s' at %s:%d\n", instr, asm_context->filename, asm_context->line);
      return -1;
    }

    if (instr_case[2]=='l')  { opcode=2<<26; }
    else { opcode=3<<26; }

    add_bin32(asm_context, opcode|operands[0].value>>2, IS_OPCODE);

    return 4;
  }

  // Coprocessor Instruction [ op 6, format 5, ft 5, fs 5, fd 5, funct 6 ]
  n=0;
  while(mips_cop_table[n].instr!=NULL)
  {
    if (strcmp(instr_case, mips_cop_table[n].instr)==0)
    {
      char shift_table[] = { 0, 5, 11, 16 };
      if (mips_cop_table[n].operand_count!=operand_count)
      {
        printf("Error: Wrong number of operands for '%s' at %s:%d\n", instr, asm_context->filename, asm_context->line);
        return -1;
      }

      opcode=(0x11<<26)|(mips_cop_table[n].format<<21)|mips_cop_table[n].function;

      for (r=0; r<operand_count; r++)
      {
        if (operands[r].type!=OPERAND_FREG)
        {
          printf("Error: '%s' expects registers at %s:%d\n", instr, asm_context->filename, asm_context->line);
          return -1;
        }
        opcode|=operands[r].value<<shift_table[(int)mips_cop_table[n].operand[r]];
      }

      add_bin32(asm_context, opcode, IS_OPCODE);
      return 4;
    }
    n++;
  }

  // I-Type?  [ op 6, rs 5, rt 5, imm 16 ]
  n=0;
  while(mips_i_table[n].instr!=NULL)
  {
    if (strcmp(instr_case, mips_i_table[n].instr)==0)
    {
      char shift_table[] = { 0, 0, 21, 16 };
      if (mips_i_table[n].operand_count!=operand_count)
      {
        printf("Error: Wrong number of operands for '%s' at %s:%d\n", instr, asm_context->filename, asm_context->line);
        return -1;
      }

      opcode=mips_i_table[n].function<<26;

      for (r=0; r<mips_i_table[n].operand_count; r++)
      {
        if ((mips_i_table[n].operand[r]==MIPS_OP_RT ||
            mips_i_table[n].operand[r]==MIPS_OP_RS) &&
            operands[r].type==OPERAND_TREG)
        {
          opcode|=operands[r].value<<shift_table[(int)mips_i_table[n].operand[r]];
        }
          else
        if (mips_i_table[n].operand[r]==MIPS_OP_LABEL)
        {
          // FIXME - Calculate address
          //if (operands[r].value>65535 || operands[r].value<-32768)
          //{
          //  print_error("Constant larger than 16 bit.", asm_context);
          //  return -1;
          //}
          opcode|=operands[r].value;
        }
          else
        if (mips_i_table[n].operand[r]==MIPS_OP_IMMEDIATE)
        {
          if (operands[r].value>65535 || operands[r].value<-32768)
          {
            print_error("Constant larger than 16 bit.", asm_context);
            return -1;
          }
          opcode|=operands[r].value;
        }
          else
        if (mips_i_table[n].operand[r]==MIPS_OP_IMMEDIATE_RS)
        {
          if (operands[r].value>65535 || operands[r].value<-32768)
          {
            print_error("Constant larger than 16 bit.", asm_context);
            return -1;
          }
          opcode|=operands[r].value;
          opcode|=operands[r].reg2<<21;
        }
          else
        if (mips_i_table[n].operand[r]==MIPS_OP_RT_IS_0)
        {
          // Derr
        }
          else
        if (mips_i_table[n].operand[r]==MIPS_OP_RT_IS_1)
        {
          opcode|=1<<16;
        }
          else
        {
          printf("Error: Illegal operand %d for '%s' at %s:%d\n", r+1, instr, asm_context->filename, asm_context->line);
          return -1;
        }
        opcode|=operands[r].value<<shift_table[(int)mips_i_table[n].operand[r]];
      }

      add_bin32(asm_context, opcode, IS_OPCODE);
      return 4;
    }
    n++;
  }

  return 4;
}


