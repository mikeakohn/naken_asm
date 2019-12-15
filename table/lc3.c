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

#include <stdio.h>
#include <stdlib.h>
#include "table/lc3.h"

struct _table_lc3 table_lc3[] =
{
  { "add",  0x1000, 0xf038, OP_R_R_R },
  { "add",  0x1020, 0xf020, OP_R_R_IMM5 },
  { "and",  0x5000, 0xf038, OP_R_R_R },
  { "and",  0x5020, 0xf020, OP_R_R_IMM5 },
  { "br",   0x0000, 0xf000, OP_BR },
  { "jmp",  0xc000, 0xfe3f, OP_BASER },
  { "jsr",  0x4800, 0xf800, OP_OFFSET11 },
  { "jsrr", 0x4000, 0xfe3f, OP_BASER },
  { "ld",   0x2000, 0xf000, OP_R_OFFSET9 },
  { "ldi",  0xa000, 0xf000, OP_R_OFFSET9 },
  { "ldr",  0x6000, 0xf000, OP_R_R_OFFSET6 },
  { "lea",  0xe000, 0xf000, OP_R_OFFSET9 },
  { "not",  0x903f, 0xf03f, OP_R_R },
  { "ret",  0xc1c0, 0xffff, OP_NONE },
  { "rti",  0x8000, 0xffff, OP_NONE },
  { "st",   0x3000, 0xf000, OP_R_OFFSET9 },
  { "sti",  0xb000, 0xf000, OP_R_OFFSET9 },
  { "str",  0x7000, 0xf000, OP_R_R_OFFSET6 },
  { "trap", 0xf000, 0xff00, OP_VECTOR },
  { NULL,   0x0000, 0x0000, OP_NONE },
};

