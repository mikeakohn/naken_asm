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

static char *arm_load_store[] =
{
  "str", "ldr"
};

static char *arm_load_store_m[] =
{
  "stm","ldm"
};

static char *arm_multiply[] =
{
  "mul","mla"
};

enum
{
  OPERAND_NOTHING,
  OPERAND_REG,
  OPERAND_REG_WRITE_BACK,
  OPERAND_IMMEDIATE,
  OPERAND_SHIFT_IMMEDIATE,
  OPERAND_SHIFT_REG,
  OPERAND_NUMBER,
  OPERAND_CONSTANT,
  OPERAND_PSR,
  OPERAND_PSRF,
  OPERAND_MULTIPLE_REG,
  OPERAND_REG_INDEXED,
  OPERAND_REG_INDEXED_OPEN,
  OPERAND_REG_INDEXED_CLOSE,
  OPERAND_IMM_INDEXED_CLOSE,
  OPERAND_SHIFT_IMM_INDEXED_CLOSE,
  OPERAND_SHIFT_REG_INDEXED_CLOSE,
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

static int compute_range(int r1, int r2)
{
  int value=0;

  if (r1==r2) { return 1<<r1; }
  if (r2<r1) { int temp=r1; r1=r2; r2=temp; }

  value=(1<<(r2+1))-1;
  if (r1!=0) { value^=(1<<r1)-1; }

  return value;
}

static int parse_alu(struct _asm_context *asm_context, struct _operand *operands, operand_count, int opcode)
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
    rn=0;
    immediate=(1<<4)|operands[1].value;
    i=1;
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
    else
  if (operand_count==3 &&
      operands[0].type==OPERAND_REG &&
      operands[1].type==OPERAND_REG &&
      operands[2].type==OPERAND_REG)
  {
    // orr rd, rn, rm
    rd=operands[0].value;
    rn=operands[1].value;
    immediate=operands[2].value;
  }
    else
  if (operand_count==4 &&
      operands[0].type==OPERAND_REG &&
      operands[1].type==OPERAND_REG &&
      operands[2].type==OPERAND_REG &&
      (operands[3].type==OPERAND_SHIFT_IMMEDIATE ||
       operands[3].type==OPERAND_SHIFT_REG))
  {
    rd=operands[0].value;
    rn=operands[1].value;

    if (operands[3].type==OPERAND_SHIFT_IMMEDIATE)
    {
      // mov rd, rn, rm, shift #
      immediate=operands[2].value|(((operands[3].value<<3)|(operands[3].sub_type<<1))<<4);
    }
      else
    {
      // mov rd, rn, rm, shift reg
      immediate=operands[2].value|(((operands[3].value<<4)|(operands[3].sub_type<<1)|1)<<4);
    }
  }
    else
  {
    print_error_illegal_operands(instr, asm_context);
    return -1;
  }

  opcode|=(i<<25)|(rn<<16)|(rd<<12)|immediate;

  add_bin32(asm_context, opcode, IS_OPCODE);
  return 4;
}

static int parse_branch(struct _asm_context *asm_context, struct _operand *operands, operand_count, int opcode)
{
  // Check for branch
  for (n=0; n<2; n++)
  {
    if (strncmp(instr_lower, arm_cond_ops[n], n+1)==0)
    {
      //if (operand_count!=1 || operands[0].type!=OPERAND_NUMBER)
      if (operand_count==1 && operands[0].type==OPERAND_NUMBER)
      {
        instr_lower+=n+1;  // It works, but ick.
        cond=parse_condition(&instr_lower);
        unsigned int offset=operands[0].value-(asm_context->address+4);

        if ((offset>>26)==0 || (offset>>26)==0x3f) { offset&=0x3ffffff; }

        if ((offset&0x3)!=0)
        {
          printf("Error: Address is not a multiple of 4 at %s:%d\n", asm_context->filename, asm_context->line);
          return -1;
        }

        //if (offset<-(1<<23) || offset>=(1<<23))
        if ((offset>>26)!=0)
        {
          print_error_range("Offset", -(1<<25), (1<<25)-1, asm_context);
          return -1;
        }

        offset>>=2;

        opcode=BRANCH_OPCODE|(n<<28)|offset;
        add_bin32(asm_context, opcode, IS_OPCODE);
        return 4;
      }
    }
  }
}

static int parse_bx(struct _asm_context *asm_context, struct _operand *operands, operand_count, int opcode)
{
  if (strncmp(instr_lower, "bx", 2)==0)
  {
    if (operand_count!=1 || operands[0].type!=OPERAND_REG)
    {
      print_error_illegal_operands(instr, asm_context);
      return -1;
    }

    instr_lower+=2;  // It works, but ick.
    cond=parse_condition(&instr_lower);
    //unsigned int offset=(asm_context->address+4)-operands[0].value;
    //if (offset<(1<<23) || offset>=(1<<23))
    opcode=BRANCH_EXCH_OPCODE|cond<<28|operands[0].value;
    add_bin32(asm_context, opcode, IS_OPCODE);

    return 4;
  }
}

static int parse_load_store(struct _asm_context *asm_context, struct _operand *operands, operand_count, int opcode)
{
  // Check for load / store.
  for (n=0; n<2; n++)
  {
    // FIXME - what to do with b, unsigned, etc
    if (strncmp(instr_lower, arm_load_store[n], 3)==0)
    {
      //int s=0;
      int offset=0;
      int i=0;
      int pr=0;
      int u=1;
      int b=0;
      int w=0;
      int ls=(*arm_load_store[n])=='s'?0:1;
      instr_lower+=3;
      cond=parse_condition(&instr_lower);

      if (instr_lower[0]=='b') { b=1; instr_lower++; }
      if (*instr_lower!=0)
      {
        print_error_unknown_instr(instr, asm_context);
        return -1;
      }

      if (operand_count==2 &&
          operands[0].type==OPERAND_REG &&
          operands[1].type==OPERAND_NUMBER)
      {
        offset=operands[1].value;
        //if (offset<0 && offset>-4096) { offset=-offset; u=0; }
        if (offset<0 || offset>4095)
        {
          print_error_range("Offset", 0, 4095, asm_context);
        }
      }
        else
      if (operand_count==2 &&
          operands[0].type==OPERAND_REG &&
          operands[1].type==OPERAND_REG_INDEXED)
      {
        offset=0;
      }
        else
      if (operand_count==3 &&
          operands[0].type==OPERAND_REG &&
          operands[1].type==OPERAND_REG_INDEXED_OPEN &&
          operands[2].type==OPERAND_IMM_INDEXED_CLOSE)
      {
        offset=operands[2].value;
        pr=1;
        if (offset<0 && offset>-4096) { offset=-offset; u=0; }
        if (offset<0 || offset>4095)
        {
          print_error_range("Offset", 0, 4095, asm_context);
        }
      }
        else
      if (operand_count==3 &&
          operands[0].type==OPERAND_REG &&
          operands[1].type==OPERAND_REG_INDEXED &&
          operands[2].type==OPERAND_IMMEDIATE)
      {
        offset=operands[2].value;
        if (offset<0 && offset>-4096) { offset=-offset; u=0; }
        if (offset<0 || offset>4095)
        {
          print_error_range("Offset", 0, 4095, asm_context);
        }
      }
        else
      if (operand_count==3 &&
          operands[0].type==OPERAND_REG &&
          operands[1].type==OPERAND_REG_INDEXED &&
          operands[2].type==OPERAND_REG)
      {
        offset=operands[2].value|(1<<4);
        i=1;
      }
        else
      if (operand_count==3 &&
          operands[0].type==OPERAND_REG &&
          operands[1].type==OPERAND_REG_INDEXED_OPEN &&
          operands[2].type==OPERAND_REG_INDEXED_CLOSE)
      {
        offset=operands[2].value|(1<<4);
        pr=1;
        i=1;
      }
        else
      if (operand_count==3 &&
          operands[0].type==OPERAND_REG &&
          operands[1].type==OPERAND_REG_INDEXED &&
          operands[2].type==OPERAND_REG &&
          (operands[3].type==OPERAND_SHIFT_IMMEDIATE ||
           operands[3].type==OPERAND_SHIFT_REG))
      {
        offset=operands[2].value;

        if (operands[3].type==OPERAND_SHIFT_IMMEDIATE)
        {
          // ldr rd, [rn], rm, shift #
          offset|=(((operands[3].value<<3)|(operands[3].sub_type<<1)|1)<<4);
        }
          else
        {
          // ldr rd, [rn], rm, shift rs 
          offset|=(((operands[3].value<<4)|(operands[3].sub_type<<1))<<4);
        }

        i=1;
      }
        else
      if (operand_count==3 &&
          operands[0].type==OPERAND_REG &&
          operands[1].type==OPERAND_REG_INDEXED_OPEN &&
          operands[2].type==OPERAND_REG &&
          (operands[3].type==OPERAND_SHIFT_IMM_INDEXED_CLOSE ||
           operands[3].type==OPERAND_SHIFT_REG_INDEXED_CLOSE))
      {
        offset=operands[2].value;
        pr=1;

        if (operands[3].type==OPERAND_SHIFT_IMM_INDEXED_CLOSE)
        {
          // ldr rd, [rn], rm, shift #
          offset|=(((operands[3].value<<3)|(operands[3].sub_type<<1)|1)<<4);
        }
          else
        {
          // ldr rd, [rn], rm, shift rs 
          offset|=(((operands[3].value<<4)|(operands[3].sub_type<<1))<<4);
        }

        i=1;
      }
        else
      {
        print_error_illegal_operands(instr, asm_context);
        return -1;
      }

      add_bin32(asm_context, LDR_STR_OPCODE|(cond<<28)|(i<<25)|(pr<<24)|(u<<23)|(b<<22)|(w<<21)|(ls<<20)|(operands[1].value<<16)|(operands[0].value<<12)|offset, IS_OPCODE);
      return 4;
    }
  }
}

static int parse_load_store_multiple(struct _asm_context *asm_context, struct _operand *operands, operand_count, int opcode)
{
  // Check for load / store multiple.
  for (n=0; n<2; n++)
  {
    if (strncmp(instr_lower, arm_load_store_m[n], 3)==0)
    {
      int pr=0;
      int u=0;
      int s=0;
      int w=(operands[0].type==OPERAND_REG_WRITE_BACK)?1:0;
      int ls=(*arm_load_store[n])=='s'?0:1;
      instr_lower+=3;
      cond=parse_condition(&instr_lower);

      if (instr_lower[0]=='s') { s=1; instr_lower++; }
      if (*instr_lower!=0)
      {
        print_error_unknown_instr(instr, asm_context);
        return -1;
      }

      if (operand_count==2 &&
          (operands[0].type==OPERAND_REG ||
           operands[0].type==OPERAND_REG_WRITE_BACK) &&
          operands[1].type==OPERAND_MULTIPLE_REG)
      {
        add_bin32(asm_context, 0x08000000|(cond<<28)|(pr<<24)|(u<<23)|(s<<22)|(w<<21)|(ls<<20)|(operands[0].value<<16)|operands[1].value, IS_OPCODE);
        return 4;
      }
    }
  }
}

static int parse_swap(struct _asm_context *asm_context, struct _operand *operands, operand_count, int opcode)
{
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
         operands[2].type==OPERAND_REG_INDEXED)
    {
      if (*instr_lower=='b') b=1;

      add_bin32(asm_context, SWAP_OPCODE|(cond<<28)|(b<<22)|(operands[2].value<<16)|(operands[0].value<<12)|operands[1].value, IS_OPCODE);
      return 4;
    }
  }
}

static int parse_mrs(struct _asm_context *asm_context, struct _operand *operands, operand_count, int opcode)
{
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
}

static int parse_msr(struct _asm_context *asm_context, struct _operand *operands, operand_count, int opcode)
{
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
}

static int parse_swi(struct _asm_context *asm_context, struct _operand *operands, operand_count, int opcode)
{
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
}

static int parse_mul(struct _asm_context *asm_context, struct _operand *operands, operand_count)
{
  // Check for MUL / MLA (multiply or multiply and accumulate)
  for (n=0; n<2; n++)
  {
    if (strncmp(instr_lower, arm_multiply[n], 3)==0)
    {
      int s=0;
      int rn;
      instr_lower+=3;
      cond=parse_condition(&instr_lower);

      if (instr_lower[0]=='s') { s=1; instr_lower++; }
      if (*instr_lower!=0)
      {
        print_error_unknown_instr(instr, asm_context);
        return -1;
      }

      if (operands[0].type!=OPERAND_REG ||
          operands[1].type!=OPERAND_REG ||
          operands[2].type!=OPERAND_REG)
      {
        print_error_illegal_operands(instr, asm_context);
        return -1;
      }

      if (operand_count==3 && n==0)
      {
        rn=0;
      }
        else
      if (operand_count==4 && n==1 &&
          operands[3].type==OPERAND_REG)
      {
        rn=operands[3].value;
      }
        else
      {
        print_error_illegal_operands(instr, asm_context);
        return -1;
      }

      add_bin32(asm_context, MUL_OPCODE|(cond<<28)|(n<<21)|(s<<20)|(operands[0].value<<16)|(operands[1].value)|(operands[2].value<<8)|(rn<<12), IS_OPCODE);
      return 4;
    }
  }
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
int matched=0;

  lower_copy(instr_lower, instr);
  memset(operands, 0, sizeof(operands));
  operand_count=0;

  // First parse instruction into the operands structures.
  while(1)
  {
    token_type=get_token(asm_context, token, TOKENLEN);
    if (token_type==TOKEN_EOL) { break; }

#if DEBUG
    printf("arm: %s  (%d)\n", token, token_type);
#endif
    n=get_register_arm(token);

    if (n!=-1)
    {
      operands[operand_count].value=n;
      operands[operand_count].type=OPERAND_REG;

      token_type=get_token(asm_context, token, TOKENLEN);
      if (IS_TOKEN(token,']'))
      {
        operands[operand_count].type=OPERAND_REG_INDEXED_CLOSE;
        token_type=get_token(asm_context, token, TOKENLEN);
      }

      if (IS_TOKEN(token,'!'))
      {
        operands[operand_count].type=OPERAND_REG_WRITE_BACK;
      }
        else
      {
        pushback(asm_context, token, token_type);
      }
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
      int num;
      operands[operand_count].type=OPERAND_IMMEDIATE;

      if (eval_expression(asm_context, &num)!=0)
      {
        if (asm_context->pass==1)
        {
          eat_operand(asm_context);
        }
          else
        {
          print_error_unexp(token, asm_context);
          return -1;
        }
      }

      operands[operand_count].value=num;

      token_type=get_token(asm_context, token, TOKENLEN);
      if (IS_TOKEN(token,']'))
      {
        operands[operand_count].type=OPERAND_IMM_INDEXED_CLOSE;
      }
        else
      {
        pushback(asm_context, token, token_type);
      }
    }
      else
    if (IS_TOKEN(token,'['))
    {
      operands[operand_count].type=OPERAND_REG_INDEXED_OPEN;
      token_type=get_token(asm_context, token, TOKENLEN);
      n=get_register_arm(token);
      if (n==-1)
      {
        print_error_unexp(token, asm_context);
        return -1;
      }

      operands[operand_count].value=n;
      token_type=get_token(asm_context, token, TOKENLEN);

      if (IS_TOKEN(token,']'))
      {
        operands[operand_count].type=OPERAND_REG_INDEXED;
      }
        else
      {
        pushback(asm_context, token, token_type);
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
    if (IS_TOKEN(token,'{'))
    {
      operands[operand_count].type=OPERAND_MULTIPLE_REG;
      operands[operand_count].value=0;
      while(1)
      {
        token_type=get_token(asm_context, token, TOKENLEN);
        int r1=get_register_arm(token);
        int r2;
        if (r1==-1)
        {
          print_error_unexp(token, asm_context);
          return -1;
        }

        token_type=get_token(asm_context, token, TOKENLEN);
        if (IS_TOKEN(token,'-'))
        {
          token_type=get_token(asm_context, token, TOKENLEN);

          r2=get_register_arm(token);
          if (r2==-1)
          {
            print_error_unexp(token, asm_context);
            return -1;
          }

          token_type=get_token(asm_context, token, TOKENLEN);
        }
        else
        {
          r2=r1;
        }

        operands[operand_count].value|=compute_range(r1,r2);

        if (IS_TOKEN(token,'}')) { break; }
        if (IS_NOT_TOKEN(token,','))
        {
          print_error_unexp(token, asm_context);
          return -1;
        }
      }
    }
      else
    {
      for (n=0; n<4; n++)
      {
        if (strcasecmp(token, arm_shift[n])==0)
        {
          token_type=get_token(asm_context, token, TOKENLEN);
          operands[operand_count].sub_type=n;

          if (IS_TOKEN(token,'#'))
          {
            token_type=get_token(asm_context, token, TOKENLEN);
            if (token_type!=TOKEN_NUMBER)
            {
              print_error_unexp(token, asm_context);
              return -1;
            }

            operands[operand_count].value=atoi(token);

            if (operands[operand_count].value<0 ||
                operands[operand_count].value>31)
            {
              print_error_range("Shift value", 0, 31, asm_context);
              return -1;
            }

            operands[operand_count].type=OPERAND_SHIFT_IMMEDIATE;
          }
            else
          {
            int r=get_register_arm(token);
            if (r==-1)
            {
              print_error_unexp(token, asm_context);
              return -1;
            }

            operands[operand_count].value=r;
            operands[operand_count].type=OPERAND_SHIFT_REG;
          }

          token_type=get_token(asm_context, token, TOKENLEN);
          if (IS_TOKEN(token,']'))
          {
            if (operands[operand_count].type==OPERAND_SHIFT_REG)
            {
              operands[operand_count].type=OPERAND_SHIFT_REG_INDEXED_CLOSE;
            }
              else
            if (operands[operand_count].type==OPERAND_SHIFT_IMMEDIATE)
            {
              operands[operand_count].type=OPERAND_SHIFT_IMM_INDEXED_CLOSE;
            }
          }
            else
          {
            pushback(asm_context, token, token_type);
          }

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

  n=0;
  while(table_arm[n].instr!=NULL)
  {
    if (strcmp(table_arm[n].instr,instr_case)==0)
    {
      matched=1;

      switch(table_arm[n].type)
      {
        case OP_ALU:
        case OP_MULTIPLY:
        case OP_SWAP:
        case OP_MRS:
        case OP_MSR_ALL:
        case OP_MSR_FLAG:
        case OP_LDR:
        case OP_STR:
        case OP_UNDEFINED:
        case OP_LDM:
        case OP_STM:
        case OP_BRANCH:
        case OP_BRANCH_EXCHANGE:
        case OP_SWI:
        default:
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


