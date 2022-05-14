/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2022 by Michael Kohn
 *
 */

#ifndef NAKEN_ASM_TABLE_8008_H
#define NAKEN_ASM_TABLE_8008_H

#include "common/assembler.h"

enum
{
  OP_NONE,
  OP_SREG,
  OP_DREG,
  OP_DREG_NOT_A,
  OP_REG_REG,
  OP_REG_M,
  OP_M_REG,
  OP_M,
  OP_ADDRESS,
  OP_IMMEDIATE,
  OP_M_IMMEDIATE,
  OP_REG_IMMEDIATE,
  OP_SUB,
  OP_PORT_MMM,
  OP_PORT_MMM_NOT_0,
};

struct _table_8008
{
  const char *instr;
  uint8_t opcode;
  uint8_t mask;
  uint8_t type;
};

extern struct _table_8008 table_8008[];

#endif

