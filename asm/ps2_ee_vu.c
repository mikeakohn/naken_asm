/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2016 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "asm/common.h"
#include "asm/ps2_ee_vu.h"
#include "common/assembler.h"
#include "common/tokens.h"
#include "common/eval_expression.h"
#include "table/ps2_ee_vu.h"

enum
{
  OPERAND_FREG,
  OPERAND_IMMEDIATE,
};

struct _operand
{
  int value;
  int type;
  int reg2;
};

#if 0
static int get_register_ps2_ee_vu(char *token, char letter)
{

  return -1;
}
#endif

int get_operands(struct _asm_context *asm_context, struct _operand *operands, char *instr, char *instr_case)
{

  return -1;
}

int parse_instruction_ps2_ee_vu(struct _asm_context *asm_context, char *instr)
{

  return -1;
}


