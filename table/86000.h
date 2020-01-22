/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2020 by Michael Kohn
 *
 */

#ifndef NAKEN_ASM_TABLE_86000_H
#define NAKEN_ASM_TABLE_86000_H

#include "common/assembler.h"

enum
{
  OP_NONE,
  OP_ADDRESS,
  OP_IMMEDIATE,
  OP_AT_REG,
  OP_ADDRESS_RELATIVE8,
  OP_IMMEDIATE_RELATIVE8,
  OP_AT_REG_IMMEDIATE_RELATIVE8,
  OP_ADDRESS_BIT_RELATIVE8,
  OP_RELATIVE8,
  OP_RELATIVE16,
  OP_ADDRESS12,
  OP_ADDRESS16,
  OP_ADDRESS_BIT,
  OP_AT_REG_RELATIVE8,
  OP_IMMEDIATE_ADDRESS,
  OP_IMMEDIATE_AT_REG,
};

struct _table_86000
{
  char *name;
  uint8_t opcode;
  uint8_t mask;
  uint8_t type;
  uint8_t cycles;
};

extern struct _table_86000 table_86000[];

#endif

