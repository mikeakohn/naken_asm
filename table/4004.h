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

#ifndef NAKEN_ASM_TABLE_4004_H
#define NAKEN_ASM_TABLE_4004_H

#include "common/assembler.h"

enum
{
  OP_NONE,
  OP_R,
  OP_P,
  OP_ADDR12,
  OP_DATA,
  OP_P_DATA,
  OP_R_ADDR8,
  OP_COND,
  OP_COND_ALIAS,
};

struct _table_4004
{
  const char *instr;
  uint8_t opcode;
  uint8_t mask;
  uint8_t type;
};

extern struct _table_4004 table_4004[];

#endif

