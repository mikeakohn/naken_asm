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

#include "table/sparc.h"

struct _table_sparc table_sparc[] =
{
  // Instructions
  { "add",    0x80000000, 0xc1f81fe0, OP_REG_REG_REG, 0 },
  { "add",    0x80001000, 0xc1f81000, OP_REG_SIMM13_REG, 0 },
  { "addcc",  0x80800000, 0xc1f81fe0, OP_REG_REG_REG, 0 },
  { "addcc",  0x80801000, 0xc1f81000, OP_REG_SIMM13_REG, 0 },
  { "addc",   0x80400000, 0xc1f81fe0, OP_REG_REG_REG, 0 },
  { "addc",   0x80401000, 0xc1f81000, OP_REG_SIMM13_REG, 0 },
  { "addccc", 0x80c00000, 0xc1f81fe0, OP_REG_REG_REG, 0 },
  { "addccc", 0x80c01000, 0xc1f81000, OP_REG_SIMM13_REG, 0 },
  { NULL, 0, 0, 0, 0 }
};

