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

#ifndef NAKEN_ASM_TABLE_ARM64_H
#define NAKEN_ASM_TABLE_ARM64_H

#include <stdint.h>

enum
{
  OP_NONE,
};

struct _table_arm64
{
  const char *instr;
  uint32_t opcode;
  uint32_t mask;
  uint8_t type;
};

extern struct _table_arm64 table_arm64[];

#endif

