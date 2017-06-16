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

#ifndef _TABLE_PROPELLER_H
#define _TABLE_PROPELLER_H

#include <stdint.h>

enum
{
  PROPELLER_OP_NONE,
  PROPELLER_OP_NOP,
  PROPELLER_OP_DS,
  PROPELLER_OP_DS_15_1,
  PROPELLER_OP_DS_15_2,
  PROPELLER_OP_S,
  PROPELLER_OP_D,
  PROPELLER_OP_IMMEDIATE,
};

struct _table_propeller
{
  const char *instr;
  uint32_t opcode;
  uint32_t mask;
  uint8_t type;
  uint8_t cycles_min;
  uint8_t cycles_max;
};

extern struct _table_propeller table_propeller[];

#endif

