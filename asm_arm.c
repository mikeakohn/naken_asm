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

static char *arm_cond_ops[] =
{
  "b", "bl"
};

enum
{
  OPERAND_REG,
  OPERAND_IMMEDIATE,
  OPERAND_INDEXED,
  OPERAND_SHIFT,
  OPERAND_NUMBER,
  OPERAND_PSR,
  OPERAND_PSRF,
};


struct _operand
{
  unsigned int value;
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

  if (strcasecmp("sp", token)==0) { return 13; }
  if (strcasecmp("lr", token)==0) { return 14; }
  if (strcasecmp("pc", token)==0) { return 15; }

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

static int compute_immediate(int immediate)
{
  unsigned int i=immediate;
  int n;

  //printf("Compute immediate\n");

  for (n=0; n<16; n++)
  {
    //printf("n=%d  i=%u\n", n, i);
    if (i<256) { return i|(n<<8); }
    //i=((i&0x3)<<30)|(i>>2);
    i=((i&0xc0000000)>>30)|(i<<2);
  }

  return -1;
}

static int imm_shift_to_immediate(struct _asm_context *asm_context, struct _operand *operands, int operand_count, int pos)
{
  if (operands[pos].value>=256 || operands[pos].value<0)
  {
    printf("Error: Immediate out of range for #imm, shift at %s:%d\n", asm_context->filename, asm_context->line);
    return -1;
  }

  if ((operands[pos+1].value&1)==1 ||
       operands[pos+1].value>30 || operands[pos+1].value<0)
  {
    printf("Error: Bad shift value for #imm, shift at %s:%d\n", asm_context->filename, asm_context->line);
    return -1;
  }

  return operands[pos].value|(((operands[pos+1].value>>1)<<8));
}

int parse_instruction_arm(struct _asm_context *asm_context, char *instr)
{
struct _operand operands[4];
int operand_count;
char instr_lower_mem[TOKENLEN];
char *instr_lower=instr_lower_mem;
char token[TOKENLEN];
int token_type;
int n,cond;
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
    if (token_type==TOKEN_NUMBER)
    {
      operands[operand_count].value=atoi(token);
      operands[operand_count].type=OPERAND_NUMBER;
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
      if (n==-1)
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
    if (strcasecmp(token, "cpsr")==0)
    {
      operands[operand_count].type=OPERAND_PSR;
      operands[operand_count].value=0;
    }
      else
    if (strcasecmp(token, "spsr")==0)
    {
      operands[operand_count].type=OPERAND_PSR;
      operands[operand_count].value=1;
    }
      else
    if (strcasecmp(token, "cpsr_flg")==0)
    {
      operands[operand_count].type=OPERAND_PSRF;
      operands[operand_count].value=0;
    }
      else
    if (strcasecmp(token, "spsr_flg")==0)
    {
      operands[operand_count].type=OPERAND_PSRF;
      operands[operand_count].value=1;
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

    if (operand_count==4)
    {
      print_error_unexp(token, asm_context);
      return -1;
    }
  }

#ifdef DEBUG
  printf("--------- new operand ----------\n");
  for (n=0; n<operand_count; n++)
  {
    printf("operand_type=%d operand_value=%d\n", operands[n].type, operands[n].value);
  }
#endif

  // Check for an ALU instruction
  for (n=0; n<16; n++)
  {
    if (strncmp(instr_lower, arm_alu_ops[n], 3)==0)
    {
      int immediate=0;
      // S flag
      int s=0;
      int rd=0,rn=0;
      int i=0;

      // Change mov rd, #0xffffffff to mvn rd, #0
      if (n==13 && operand_count==2 && operands[0].type==OPERAND_REG &&
          operands[1].type==OPERAND_IMMEDIATE && operands[1].value==0xffffffff)
      {
        strncpy(instr_lower, "mvn", 3);
      }

      instr_lower+=3;

      cond=parse_condition(&instr_lower);

      if (instr_lower[0]=='s') { s=1; instr_lower++; }
      // According to some doc, tst, teq, cmp, cmn should set S all the time
      if (n>=8 && n<12) { s=1; }

      if (*instr_lower!=0)
      {
        print_error_unknown_instr(instr, asm_context);
        return -1;
      }

      opcode=(cond<<28)|(s<<20)|(n<<21);

      if (operand_count==2 &&
          operands[0].type==OPERAND_REG &&
          operands[1].type==OPERAND_REG)
      {
        // mov rd, rn
        rd=operands[0].value;
        rn=operands[1].value;
      }
        else
      if (operand_count==2 &&
          operands[0].type==OPERAND_REG &&
          operands[1].type==OPERAND_IMMEDIATE)
      {
        // mov rd, #imm
        rd=operands[0].value;
        immediate=compute_immediate(operands[1].value);
        i=1;
      }
        else
      if (operand_count==3 &&
          operands[0].type==OPERAND_REG &&
          operands[1].type==OPERAND_IMMEDIATE &&
          operands[2].type==OPERAND_NUMBER)
      {
        // mov rd, #imm, shift
        rd=operands[0].value;
        immediate=imm_shift_to_immediate(asm_context, operands, operand_count, 1);
        i=1;
        if (immediate<0) { return -1; }
      }
        else
      if (operand_count==3 &&
          operands[0].type==OPERAND_REG &&
          operands[1].type==OPERAND_REG &&
          operands[2].type==OPERAND_IMMEDIATE)
      {
        // mov rd, rn, #imm
        rd=operands[0].value;
        rn=operands[1].value;
        immediate=compute_immediate(operands[2].value);
        i=1;
      }
        else
      if (operand_count==4 &&
          operands[0].type==OPERAND_REG &&
          operands[1].type==OPERAND_REG &&
          operands[2].type==OPERAND_IMMEDIATE &&
          operands[3].type==OPERAND_NUMBER)
      {
        // mov rd, rn, #imm, shift
        rd=operands[0].value;
        rn=operands[1].value;
        immediate=imm_shift_to_immediate(asm_context, operands, operand_count, 2);
        i=1;
        if (immediate<0) { return -1; }
      }

      opcode|=(i<<25)|(rn<<16)|(rd<<12)|immediate;

      add_bin32(asm_context, opcode, IS_OPCODE);
      return 4;

    }
  }

  // Check for branch
  for (n=0; n<2; n++)
  {
    if (strncmp(instr_lower, arm_cond_ops[n], 1)==0)
    {
      if (operand_count!=1 || operands[0].type!=OPERAND_NUMBER)
      {
        print_error_illegal_operands(instr, asm_context);
      }

      instr_lower+=n+1;  // It works, but ick.
      cond=parse_condition(&instr_lower);
      unsigned int offset=(asm_context->address+4)-operands[0].value;
      if (offset<(1<<23) || offset>=(1<<23))
      opcode=0xa0000000|(n<<28)|offset;
      add_bin32(asm_context, opcode, IS_OPCODE);
    }
  }

  // Check for swap
  if (strncmp(instr_lower, "swp", 3)==0)
  {
    // B flag
    int b=0;

    instr_lower+=3;
    cond=parse_condition(&instr_lower);

    if ((*instr_lower=='b' || *instr_lower==0) &&
         operand_count==3 &&
         operands[0].type==OPERAND_REG &&
         operands[1].type==OPERAND_REG &&
         operands[2].type==OPERAND_INDEXED)
    {
      if (*instr_lower=='b') b=1;

      add_bin32(asm_context, SWAP_OPCODE|(cond<<28)|(b<<22)|(operands[2].value<<16)|(operands[0].value<<12)|operands[1].value, IS_OPCODE);
      return 4;
    }
  }

  // Check for mrs
  if (strncmp(instr_lower, "mrs", 3)==0)
  {
    // PS flag
    int ps=0;

    instr_lower+=3;
    cond=parse_condition(&instr_lower);

    if (*instr_lower==0 && operand_count==2 &&
        operands[0].type==OPERAND_REG &&
        operands[1].type==OPERAND_PSR)
    {
      ps=operands[1].value;
      add_bin32(asm_context, MRS_OPCODE|(cond<<28)|(ps<<22)|(operands[0].value<<12), IS_OPCODE);
      return 4;
    }
  }

  // Check for msr
  if (strncmp(instr_lower, "msr", 3)==0)
  {
    // PS flag
    int ps=0;

    instr_lower+=3;
    cond=parse_condition(&instr_lower);

    if (*instr_lower==0 && operand_count==2 && operands[0].type==OPERAND_PSR)
    {
      if (operands[1].type==OPERAND_REG)
      {
        ps=operands[0].value;
        add_bin32(asm_context, MSR_ALL_OPCODE|(cond<<28)|(ps<<22)|(operands[1].value<<12), IS_OPCODE);
        return 4;
      }
        else
      if (operands[1].type==OPERAND_REG)
      {
        ps=operands[0].value;
        add_bin32(asm_context, MSR_FLAG_OPCODE|(cond<<28)|(ps<<22)|(operands[1].value), IS_OPCODE);
        return 4;
      }
        else
      if (operands[1].type==OPERAND_IMMEDIATE)
      {
        ps=operands[0].value;
        int source_operand=compute_immediate(operands[1].value);
        if (source_operand==-1)
        {
          printf("Error: Can't create a constant for immediate value %d at %s:%d\n", operands[1].value, asm_context->filename, asm_context->line);
          return -1;
        }

        add_bin32(asm_context, MSR_FLAG_OPCODE|(cond<<28)|(1<<25)|(ps<<22)|source_operand, IS_OPCODE);
        return 4;
      }
    }
      else
    if (operand_count==3 &&
        operands[0].type==OPERAND_PSR &&
        operands[1].type==OPERAND_IMMEDIATE &&
        operands[2].type==OPERAND_NUMBER)
    {
      int source_operand=imm_shift_to_immediate(asm_context, operands, operand_count, 1);
      if (source_operand<0) { return -1; }
      add_bin32(asm_context, MSR_FLAG_OPCODE|(cond<<28)|(1<<25)|(ps<<22)|source_operand, IS_OPCODE);
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

      add_bin32(asm_context, CO_SWI_OPCODE|(cond<<28), IS_OPCODE);
      return 4;
    }
  }

  print_error_unknown_instr(instr, asm_context);

  return -1;
}


