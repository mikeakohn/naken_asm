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
  { "cbd",    0x3e800000, 0xffe00000, OP_RT_I7_RA,     FLAG_NONE, 0, 0 },
  { "cbx",    0x3a800000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "cdd",    0x3ee00000, 0xffe00000, OP_RT_I7_RA,     FLAG_NONE, 0, 0 },
  { "cdx",    0x3ae00000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "chd",    0x3ea00000, 0xffe00000, OP_RT_I7_RA,     FLAG_NONE, 0, 0 },
  { "chx",    0x3aa00000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "cwd",    0x3ec00000, 0xffe00000, OP_RT_I7_RA,     FLAG_NONE, 0, 0 },
  { "cwx",    0x3ac00000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "il",     0x40800000, 0xff800000, OP_RT_SIGNED16,  FLAG_NONE, 0, 0 },
  { "ilh",    0x41800000, 0xff800000, OP_RT_UNSIGNED16,FLAG_NONE, 0, 0 },
  { "ilhu",   0x41000000, 0xff800000, OP_RT_UNSIGNED16,FLAG_NONE, 0, 0 },
  { "lqa",    0x30800000, 0xff800000, OP_RT_ADDRESS,   FLAG_NONE, 0, 0 },
  { "lqd",    0x34000000, 0xff000000, OP_RT_I10_RA,    FLAG_NONE, 0, 0 },
  { "lqr",    0x33800000, 0xff800000, OP_RT_RELATIVE,  FLAG_NONE, 0, 0 },
  { "lqx",    0x38800000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "stqa",   0x20800000, 0xff800000, OP_RT_ADDRESS,   FLAG_NONE, 0, 0 },
  { "stqd",   0x24000000, 0xff000000, OP_RT_I10_RA,    FLAG_NONE, 0, 0 },
  { "stqr",   0x23800000, 0xff800000, OP_RT_RELATIVE,  FLAG_NONE, 0, 0 },
  { "stqx",   0x28800000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { NULL, 0, 0, 0, 0 }
};

