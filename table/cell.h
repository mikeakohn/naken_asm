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

#ifndef _TABLE_POWERPC_H
#define _TABLE_POWERPC_H

#include "common/assembler.h"

enum
{
  OP_NONE,
  OP_RT_SYMBOL_RA,
  OP_RT_RA_RB,
  OP_RT_SYMBOL,
};

struct _table_cell
{
  char *instr;
  uint32_t opcode;
  uint32_t mask;
  uint8_t type;
  uint8_t flags;
  uint8_t cycles;
  uint8_t cycles_max;
};

#define FLAG_NONE 0

extern struct _table_cell table_cell[];

#endif


