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

#ifndef NAKEN_ASM_TABLE_SPARC_H
#define NAKEN_ASM_TABLE_SPARC_H

#include "common/assembler.h"

enum
{
  OP_NONE,
  OP_REG_REG_REG,
  OP_REG_SIMM13_REG,
};

struct _table_sparc
{
  char *instr;
  uint32_t opcode;
  uint32_t mask;
  uint8_t type;
  uint8_t flags;
};

extern struct _table_sparc table_sparc[];

#endif

