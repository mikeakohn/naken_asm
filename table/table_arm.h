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

#ifndef _TABLE_ARM_H
#define _TABLE_ARM_H

#include <stdint.h>

#include "assembler.h"

enum
{
  OP_SHIFT,
};

struct _table_arm
{
  char *instr;
  uint32_t opcode;
  uint32_t mask;
  uint8_t type;
  int8_t cycles;
};

extern struct _table_arm table_arm[];

#endif


