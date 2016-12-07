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
  { "a",      0x18000000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "ai",     0x1c000000, 0xff000000, OP_RT_S10_RA,    FLAG_NONE, 0, 0 },
  { "ah",     0x19000000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "ahi",    0x1d000000, 0xff000000, OP_RT_S10_RA,    FLAG_NONE, 0, 0 },
  { "addx",   0x68000000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "bg",     0x08400000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "bgx",    0x68600000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "cbd",    0x3e800000, 0xffe00000, OP_RT_S7_RA,     FLAG_NONE, 0, 0 },
  { "cbx",    0x3a800000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "cdd",    0x3ee00000, 0xffe00000, OP_RT_S7_RA,     FLAG_NONE, 0, 0 },
  { "cdx",    0x3ae00000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "cg",     0x18400000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "cgx",    0x68400000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "chd",    0x3ea00000, 0xffe00000, OP_RT_S7_RA,     FLAG_NONE, 0, 0 },
  { "chx",    0x3aa00000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "cwd",    0x3ec00000, 0xffe00000, OP_RT_S7_RA,     FLAG_NONE, 0, 0 },
  { "cwx",    0x3ac00000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "fsmbi",  0x32800000, 0xff800000, OP_RT_U16,       FLAG_NONE, 0, 0 },
  { "il",     0x40800000, 0xff800000, OP_RT_U16,       FLAG_NONE, 0, 0 },
  { "ilh",    0x41800000, 0xff800000, OP_RT_U16,       FLAG_NONE, 0, 0 },
  { "ilhu",   0x41000000, 0xff800000, OP_RT_U16,       FLAG_NONE, 0, 0 },
  { "ila",    0x42000000, 0xfe000000, OP_RT_U18,       FLAG_NONE, 0, 0 },
  { "iohl",   0x60800000, 0xff800000, OP_RT_U16,       FLAG_NONE, 0, 0 },
  { "lqa",    0x30800000, 0xff800000, OP_RT_ADDRESS,   FLAG_NONE, 0, 0 },
  { "lqd",    0x34000000, 0xff000000, OP_RT_S10_RA,    FLAG_NONE, 0, 0 },
  { "lqr",    0x33800000, 0xff800000, OP_RT_RELATIVE,  FLAG_NONE, 0, 0 },
  { "lqx",    0x38800000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "mpy",    0x78800000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "mpyi",   0x74000000, 0xff000000, OP_RT_S10_RA,    FLAG_NONE, 0, 0 },
  { "mpyu",   0x79800000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "mpyui",  0x75000000, 0xff000000, OP_RT_S10_RA,    FLAG_NONE, 0, 0 },
  { "sf",     0x08000000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "sfi",    0x0c000000, 0xff000000, OP_RT_S10_RA,    FLAG_NONE, 0, 0 },
  { "sfh",    0x09000000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "sfx",    0x68200000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "shfi",   0x0d000000, 0xff000000, OP_RT_S10_RA,    FLAG_NONE, 0, 0 },
  { "stqa",   0x20800000, 0xff800000, OP_RT_ADDRESS,   FLAG_NONE, 0, 0 },
  { "stqd",   0x24000000, 0xff000000, OP_RT_S10_RA,    FLAG_NONE, 0, 0 },
  { "stqr",   0x23800000, 0xff800000, OP_RT_RELATIVE,  FLAG_NONE, 0, 0 },
  { "stqx",   0x28800000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { NULL, 0, 0, 0, 0 }
};

