/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2022 by Michael Kohn
 *
 */

#ifndef NAKEN_ASM_ASM_PDK_PARSE_H
#define NAKEN_ASM_ASM_PDK_PARSE_H

#include "common/assembler.h"

#define MAX_OPERANDS 2

enum
{ 
  OPERAND_ADDRESS,
  OPERAND_IMMEDIATE,
  OPERAND_A,
  OPERAND_BIT_OFFSET,
  OPERAND_ADDRESS_BIT_OFFSET,
};

struct _operand
{
  int value;
  int type;
  int bit;
};

int pdk_parse(struct _asm_context *asm_context, struct _operand *operands);

#endif

