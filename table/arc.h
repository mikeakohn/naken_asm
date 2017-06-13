/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2017 by Michael Kohn
 *
 */

#ifndef _TABLE_ARC_H
#define _TABLE_ARC_H

#include "common/assembler.h"

enum
{
  OP_NONE,
  OP_B_C,
  OP_B_U6,
  OP_B_LIMM,
  OP_0_C,
  OP_0_U6,
  OP_0_LIMM,
};

struct _table_arc
{
  char *instr;
  uint32_t opcode;
  uint32_t mask;
  uint8_t type;
};

struct _table_arc16
{
  char *instr;
  uint16_t opcode;
  uint16_t mask;
  uint8_t type;
};

extern struct _table_arc table_arc[];
extern struct _table_arc16 table_arc16[];

#endif


