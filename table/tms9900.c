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

#include "table/tms9900.h"

struct _table_tms9900 table_tms9900[] =
{
  { "a", 0xa000, OP_DUAL, 14, 14 },
  { "ab", 0xb000, OP_DUAL, 14, 14 },
  { "abs", 0x0740, OP_DUAL, 12, 14 },
  { "ai", 0x0220, OP_IMMEDIATE, 14, 14 },
  { "andi", 0x0240, OP_IMMEDIATE, 14, 14 },
  { "b", 0x0440, OP_SINGLE, 8, 8 },
  { "bl", 0x0680, OP_SINGLE, 12, 12 },
  { "blwp", 0x0400, OP_SINGLE, 26, 26 },
  { "c", 0x8000, OP_DUAL, 14, 14 },
  { "cb", 0x9000, OP_DUAL, 14, 14 },
  { "ci", 0x0280, OP_IMMEDIATE, 14, 14 },
  { "ckof", 0x03c0, OP_EXTERNAL, 12, 12 },
  { "ckon", 0x03a0, OP_EXTERNAL, 12, 12 },
  { "clr", 0x04c0, OP_SINGLE, 10, 10 },
  { "coc", 0x2000, OP_DUAL_MULTIPLE, 14, 14 },
  { "czc", 0x2400, OP_DUAL_MULTIPLE, 14, 14 },
  { "dec", 0x0600, OP_SINGLE, 10, 10 },
  { "dect", 0x0640, OP_SINGLE, 10, 10 },
  { "div", 0x3c00, OP_DUAL_MULTIPLE, 16, 124 },
  { "idle", 0x0340, OP_EXTERNAL, 12, 12 },
  { "inc", 0x0580, OP_SINGLE, 10, 10 },
  { "inct", 0x05c0, OP_SINGLE, 10, 10 },
  { "inv", 0x0540, OP_SINGLE, 10, 10 },
  { "jeq", 0x1300, OP_JUMP, 8, 10 },
  { "jgt", 0x1500, OP_JUMP, 8, 10 },
  { "jh", 0x1b00, OP_JUMP, 8, 10 },
  { "jhe", 0x1400, OP_JUMP, 8, 10 },
  { "jl", 0x1a00, OP_JUMP, 8, 10 },
  { "jle", 0x1200, OP_JUMP, 8, 10 },
  { "jlt", 0x1100, OP_JUMP, 8, 10 },
  { "jmp", 0x1000, OP_JUMP, 8, 10 },
  { "jnc", 0x1700, OP_JUMP, 8, 10 },
  { "jne", 0x1600, OP_JUMP, 8, 10 },
  { "jno", 0x1900, OP_JUMP, 8, 10 },
  { "jdc", 0x1800, OP_JUMP, 8, 10 },
  { "jop", 0x1c00, OP_JUMP, 8, 10 },
  { "ldcr", 0x3000, OP_CRU_MULTIBIT, 20, 52 },
  { "li", 0x0200, OP_IMMEDIATE, 12, 12 },
  { "limi", 0x0300, OP_INT_REG_LD, 16, 16 },
  { "lrex", 0x03e0, OP_EXTERNAL, 12, 12 },
  { "lwpi", 0x02e0, OP_INT_REG_LD, 10, 10 },
  { "mov", 0xc000, OP_DUAL, 14, 14 },
  { "movb", 0xd000, OP_DUAL, 14, 14 },
  { "mpy", 0x3800, OP_DUAL_MULTIPLE, 52, 52 },
  { "neg", 0x0500, OP_SINGLE, 12, 12 },
  { "ori", 0x0260, OP_IMMEDIATE, 14, 14 },
  { "rset", 0x0360, OP_EXTERNAL, 12, 12 },
  { "rtwp", 0x0380, OP_RTWP, 14, 14 },
  { "s", 0x6000, OP_DUAL, 14, 14 },
  { "sb", 0x7000, OP_DUAL, 14, 14 },
  { "sbo", 0x1d00, OP_CRU_SINGLEBIT, 12, 12 },
  { "sbz", 0x1e00, OP_CRU_SINGLEBIT, 12, 12 },
  { "seto", 0x0700, OP_SINGLE, 10, 10 },
  { "sla", 0x0a00, OP_SHIFT, 12, 52 },
  { "sra", 0x0800, OP_SHIFT, 12, 52 },
  { "src", 0x0b00, OP_SHIFT, 12, 52 },
  { "srl", 0x0900, OP_SHIFT, 12, 52 },
  { "soc", 0xe000, OP_DUAL, 14, 14 },
  { "socb", 0xf000, OP_DUAL, 14, 14 },
  { "stcr", 0x3400, OP_CRU_MULTIBIT, 42, 60 },
  { "stst", 0x02c0, OP_INT_REG_ST, 8, 8 },
  { "stwp", 0x02a0, OP_INT_REG_ST, 8, 8 },
  { "swpb", 0x06c0, OP_SINGLE, 10, 10 },
  { "szc", 0x4000, OP_DUAL, 14, 14 },
  { "szcb", 0x5000, OP_DUAL, 14, 14 },
  { "tb", 0x1f00, OP_CRU_SINGLEBIT, 12, 12 },
  { "x", 0x0480, OP_SINGLE, 8, 8 },
  { "xop", 0x2c00, OP_XOP, 36, 36 },
  { "xor", 0x2800, OP_DUAL_MULTIPLE, 14, 14 },
  { NULL, 0, 0, 0, 0 }
};

