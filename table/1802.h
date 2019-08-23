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

#ifndef NAKEN_ASM_TABLE_1802_H
#define NAKEN_ASM_TABLE_1802_H

#include <stdint.h>

enum
{
  RCA1802_OP_ILLEGAL,
  RCA1802_OP_NONE,
  RCA1802_OP_REG,
  RCA1802_OP_NUM_1_TO_7,
  RCA1802_OP_IMMEDIATE,
  RCA1802_OP_BRANCH,
  RCA1802_OP_LONG_BRANCH,
  RCA1802_OP_REG_BRANCH,
};

struct _table_1802
{
  const char *instr;
  uint8_t opcode;
  uint8_t mask;
  uint8_t type;
  uint8_t cycles;
};

extern struct _table_1802 table_1802[];
extern struct _table_1802 table_1802_16[];

#endif

