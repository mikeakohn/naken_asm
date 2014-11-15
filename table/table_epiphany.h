/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2014 by Michael Kohn
 *
 */

#ifndef _TABLE_EPIPHANY_H
#define _TABLE_EPIPHANY_H

#include "assembler.h"

struct _table_epiphany
{
  char *instr;
  uint32_t opcode;
  uint32_t mask;
  uint8_t type;
  uint8_t size;
  uint8_t cycles_min;
  uint8_t cycles_max;
};

enum
{
  OP_NONE,
  OP_BRANCH,
  OP_REG_3_16,
  OP_REG_3_32,
};

extern struct _table_epiphany table_epiphany[];

#endif


