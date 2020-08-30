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
  { "add",     0x80000000, 0xc1f83fe0, OP_REG_REG_REG, 0 },
  { "add",     0x80002000, 0xc1f82000, OP_REG_SIMM13_REG, 0 },
  { "addcc",   0x80800000, 0xc1f83fe0, OP_REG_REG_REG, 0 },
  { "addcc",   0x80802000, 0xc1f82000, OP_REG_SIMM13_REG, 0 },
  { "addc",    0x80400000, 0xc1f83fe0, OP_REG_REG_REG, 0 },
  { "addc",    0x80402000, 0xc1f82000, OP_REG_SIMM13_REG, 0 },
  { "addccc",  0x80c00000, 0xc1f83fe0, OP_REG_REG_REG, 0 },
  { "addccc",  0x80c02000, 0xc1f82000, OP_REG_SIMM13_REG, 0 },
  { "addxc",   0x80300220, 0xc1f83fe0, OP_REG_REG_REG, 0 },
  { "addxccc", 0x80300260, 0xc1f83fe0, OP_REG_REG_REG, 0 },
  { NULL, 0, 0, 0, 0 }
};

uint32_t mask_sparc[] =
{
  0xffffffff,                                         // OP_NONE
  (2 << 30) | (0x3f << 19) | (1 << 13) | (0xff << 5), // OP_REG_REG_REG
  (2 << 30) | (0x3f << 19) | (1 << 13),               // OP_REG_SIMM13_REG
};

