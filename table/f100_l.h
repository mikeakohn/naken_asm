/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2023 by Michael Kohn
 *
 */

#ifndef NAKEN_ASM_TABLE_F100_L_H
#define NAKEN_ASM_TABLE_F100_L_H

#include "common/assembler.h"

enum
{
  OP_NONE,
  OP_ALU,
  OP_BIT,
  OP_HALT,
  OP_INC,
  OP_COND_JMP,
  OP_SHIFT,
  OP_SHIFT_D,
};

struct _table_f100_l
{
  const char *instr;
  uint16_t opcode;
  uint16_t mask;
  uint8_t type;
};

extern struct _table_f100_l table_f100_l[];

#endif

