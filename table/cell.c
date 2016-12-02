/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2016 by Michael Kohn
 *
 */

#include "table/cell.h"

struct _table_cell table_cell[] =
{
  { "lqd",    0x34000000, 0xff000000, OP_RT_I10_RA,    FLAG_NONE, 0, 0 },
  { "lqx",    0x38800000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "lqa",    0x30800000, 0xff800000, OP_RT_SYMBOL,    FLAG_NONE, 0, 0 },
  { "lqr",    0x33800000, 0xff800000, OP_RT_RELATIVE,  FLAG_NONE, 0, 0 },
  { "stqd",   0x24000000, 0xff000000, OP_RT_I10_RA,    FLAG_NONE, 0, 0 },
  { "stqx",   0x28800000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "stqa",   0x20800000, 0xff800000, OP_RT_SYMBOL,    FLAG_NONE, 0, 0 },
  { "stqr",   0x23800000, 0xff800000, OP_RT_RELATIVE,  FLAG_NONE, 0, 0 },
  { "cbd",    0x3e800000, 0xffe00000, OP_RT_I7_RA,     FLAG_NONE, 0, 0 },
  { NULL, 0, 0, 0, 0 }
};

