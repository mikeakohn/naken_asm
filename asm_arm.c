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
  "hi", "ls" "ge", "lt",
  "gt", "le", "al", "nv"
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

int parse_instruction_arm(struct _asm_context *asm_context, char *instr)
{
char instr_lower_mem[TOKENLEN];
char *instr_lower=instr_lower_mem;
char token[TOKENLEN];
int token_type;
int n,cond,s=0;
int opcode=0;

  lower_copy(instr_lower, instr);

  for (n=0; n<16; n++)
  {
    if (strncmp(instr_lower, arm_alu_ops[n], 3)==0)
    {
      instr_lower+=3;

      for (cond=0; cond<16; cond++)
      {
        if (strncmp(instr_lower, arm_cond_a[cond], 2)==0)
        { instr_lower+=2; break; }
      }
      if (cond==16) { cond=14; }

      if (instr_lower[0]=='s') { s=1; }

      token_type=get_token(asm_context, token, TOKENLEN);
      int rd=get_register_arm(token);

      opcode=ALU_OPCODE|(n<<21)|(s<<20)|(rd<<12);

    }
  }

  return 0;
}


