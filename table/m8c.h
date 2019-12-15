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

#ifndef NAKEN_ASM_TABLE_M8C_H
#define NAKEN_ASM_TABLE_M8C_H

#include "common/assembler.h"

enum
{
  OP_NONE,
};

struct _table_m8c
{
  char *instr;
  uint8_t opcode;
  uint8_t mask;
  uint8_t type;
  uint8_t byte_count;
  uint8_t cycles;
};

extern struct _table_m8c table_m8c[];

#endif

