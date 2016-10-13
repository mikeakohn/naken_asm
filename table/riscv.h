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

#ifndef _TABLE_RISCV_H
#define _TABLE_RISCV_H

#include "common/assembler.h"

enum
{
  OP_NONE,
  OP_BRANCH,
};

struct _table_riscv
{
  char *instr;
  uint32_t opcode;
  uint32_t mask;
  uint8_t type;
  uint8_t cycles_min;
  uint8_t cycles_max;
};

extern struct _table_riscv table_riscv[];

#endif


