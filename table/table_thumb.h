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

#ifndef _TABLE_THUMB_H
#define _TABLE_THUMB_H

#include <stdint.h>

#include "assembler.h"

enum
{
  OP_SHIFT,
  OP_ADD_SUB,
  OP_IMM,
  OP_ALU,
};

struct _table_thumb
{
  char *instr;
  uint16_t opcode;
  uint16_t mask;
  uint8_t type;
  uint8_t cycles;
};

extern struct _table_thumb table_thumb[];

#endif


