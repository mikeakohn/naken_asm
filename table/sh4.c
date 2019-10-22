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
#include "table/sh4.h"

struct _table_sh4 table_sh4[] =
{
  { "add",   0x300c, 0xf00f, OP_REG_REG },
  { "add",   0x7000, 0xf000, OP_IMM_REG },
  { "addc",  0x300e, 0xf00f, OP_REG_REG },
  { "addv",  0x300f, 0xf00f, OP_REG_REG },
  { "and",   0x2009, 0xf00f, OP_REG_REG },
  { "and",   0xc900, 0xff00, OP_IMM_R0 },
  { "and.b", 0xcd00, 0xff00, OP_IMM_AT_R0_GBR },
  { "bf",    0x8b00, 0xff00, OP_BRANCH_S8 },
  { NULL,    0x0000, 0x0000, 0 },
};

