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

#include "table/xtensa.h"

// Little endian
struct _table_xtensa table_xtensa_le[] =
{
  // Instructions
  { "addi",   0x00c002, 0x00f00f, XTENSA_OP_AT_AS_IMM8, 24, 0, 0 },
  { NULL, 0, 0, 0, 0, 0 }
};

// Big endian
struct _table_xtensa table_xtensa_be[] =
{
  // Instructions
  { "addi",   0x200c00, 0xf00f00, XTENSA_OP_AT_AS_IMM8, 24, 0, 0 },
  { NULL, 0, 0, 0, 0, 0 }
};

