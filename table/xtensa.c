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

struct _table_xtensa table_xtensa[] =
{
  // Instructions
  { "abs",   0x060100, 0xff0f0f, 0x001006, 0xf0f0ff, 24, XTENSA_OP_AR_AT },
  { "abs.s", 0xfa0010, 0xff00ff, 0x0100af, 0xff00ff, 24, XTENSA_OP_FR_FS },
  { "addi",  0x00c002, 0x00f00f, 0x200c00, 0xf00f00, 24, XTENSA_OP_AT_AS_IMM8 },
  { NULL, 0, 0, 0, 0, 0 }
};

