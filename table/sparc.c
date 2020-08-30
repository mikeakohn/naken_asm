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
  { "add",                 0x80000000, OP_REG_REG_REG, 0 },
  { "add",                 0x80002000, OP_REG_SIMM13_REG, 0 },
  { "addcc",               0x80800000, OP_REG_REG_REG, 0 },
  { "addcc",               0x80802000, OP_REG_SIMM13_REG, 0 },
  { "addc",                0x80400000, OP_REG_REG_REG, 0 },
  { "addc",                0x80402000, OP_REG_SIMM13_REG, 0 },
  { "addccc",              0x80c00000, OP_REG_REG_REG, 0 },
  { "addccc",              0x80c02000, OP_REG_SIMM13_REG, 0 },
  { "addxc",               0x80300220, OP_REG_REG_REG, 0 },
  { "addxccc",             0x80300260, OP_REG_REG_REG, 0 },
  { "aes_eround01",        0x00000000, OP_FREG_FREG_FREG_FREG, 0 },
  { "aes_eround23",        0x00000000, OP_FREG_FREG_FREG_FREG, 0 },
  { "aes_dround01",        0x00000000, OP_FREG_FREG_FREG_FREG, 0 },
  { "aes_dround23",        0x00000000, OP_FREG_FREG_FREG_FREG, 0 },
  { "aes_eround01_last",   0x00000000, OP_FREG_FREG_FREG_FREG, 0 },
  { "aes_eround23_last",   0x00000000, OP_FREG_FREG_FREG_FREG, 0 },
  { "aes_dround01_last",   0x00000000, OP_FREG_FREG_FREG_FREG, 0 },
  { "aes_dround23_last",   0x00000000, OP_FREG_FREG_FREG_FREG, 0 },
  { "aes_kexpand1",        0x00000000, OP_FREG_FREG_IMM5_FREG, 0 },
  { "aes_kexpand0",        0x00000000, OP_FREG_FREG_FREG, 0 },
  { "aes_kexpand2",        0x00000000, OP_FREG_FREG_FREG, 0 },
  { "alignaddress",        0x81b00300, OP_REG_REG_REG, 0 },
  { "alignaddress_little", 0x81b00300, OP_REG_REG_REG, 0 },
  { "allclean",            0x85880000, OP_NONE, 0 },
  { NULL, 0, 0, 0 }
};

uint32_t mask_sparc[] =
{
  0xffffffff,                                         // OP_NONE
  (2 << 30) | (0x3f << 19) | (1 << 13) | (0xff << 5), // OP_REG_REG_REG
  (2 << 30) | (0x3f << 19) | (1 << 13),               // OP_REG_SIMM13_REG
  (2 << 30) | (0x3f << 19) | (0xf << 5),              // OP_FREG_FREG_FREG_FREG
  (2 << 30) | (0x3f << 19) | (0xf << 5),              // OP_FREG_FREG_IMM5_FREG
  (2 << 30) | (0x3f << 19) | (0x1ff << 5),            // OP_FREG_FREG_FREG
  (2 << 30) | (0x3f << 19) | (1 << 13) | (0xff << 5), // OP_FREG_FREG_FREG
};

