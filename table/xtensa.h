/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2019 by Michael Kohn
 *
 */

#ifndef NAKEN_ASM_TABLE_XTENSA_H
#define NAKEN_ASM_TABLE_XTENSA_H

#include "common/assembler.h"

enum
{
  XTENSA_OP_NONE,
  XTENSA_OP_AT_AS_IMM8,
};

struct _table_xtensa
{
  char *instr;
  uint32_t opcode;
  uint32_t mask;
  uint8_t type;
  uint8_t bits;
  uint8_t cycles;
  uint8_t cycles_max;
};

extern struct _table_xtensa table_xtensa_le[];
extern struct _table_xtensa table_xtensa_be[];

#endif


