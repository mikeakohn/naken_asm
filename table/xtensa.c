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
  { "abs",   0x060100, 0x001006, 24, XTENSA_OP_AR_AT },
  { "abs.s", 0xfa0010, 0x0100af, 24, XTENSA_OP_FR_FS },
  { "add",   0x800000, 0x000008, 24, XTENSA_OP_AR_AS_AT },
  { "addi",  0x00c002, 0x200c00, 24, XTENSA_OP_AT_AS_I8 },
  { "add.n", 0x00000a, 0x00a000, 16, XTENSA_OP_N_AR_AS_AT },
  { "add.s", 0x0a0000, 0x0000a0, 24, XTENSA_OP_FR_FS_FT },
  { "addi.n",0x00000b, 0x00b000, 16, XTENSA_OP_N_AR_AS_I4 },
  { "addmi", 0x00d002, 0x200d00, 24, XTENSA_OP_AT_AS_IM8 },
  { "addx2", 0x900000, 0x000009, 24, XTENSA_OP_AR_AS_AT },
  { "addx4", 0xa00000, 0x00000a, 24, XTENSA_OP_AR_AS_AT },
  { "addx8", 0xb00000, 0x00000b, 24, XTENSA_OP_AR_AS_AT },
  { "all4",  0x009000, 0x000900, 24, XTENSA_OP_BT_BS4 },
  { "all8",  0x00b000, 0x000b00, 24, XTENSA_OP_BT_BS8 },
  { "and",   0x100000, 0x000001, 24, XTENSA_OP_AR_AS_AT },
  { "andb",  0x020000, 0x000020, 24, XTENSA_OP_BR_BS_BT },
  { "andbc", 0x120000, 0x000021, 24, XTENSA_OP_BR_BS_BT },
  { "any4",  0x008000, 0x000800, 24, XTENSA_OP_BT_BS4 },
  { "any8",  0x00a000, 0x000a00, 24, XTENSA_OP_BT_BS8 },
  { "ball",  0x004007, 0x700400, 24, XTENSA_OP_BRANCH_I8 },
  { "bany",  0x008007, 0x700800, 24, XTENSA_OP_BRANCH_I8 },
  { "bbc",   0x005007, 0x700500, 24, XTENSA_OP_BRANCH_I8 },
  { "bbci",  0x006007, 0x700600, 24, XTENSA_OP_BRANCH_B5_I8 },
  { "bbci.l",0x006007, 0x700600, 24, XTENSA_OP_BRANCH_B5_I8 },
  { "bbs",   0x00d007, 0x700d00, 24, XTENSA_OP_BRANCH_I8 },
  { "bbsi",  0x00e007, 0x700e00, 24, XTENSA_OP_BRANCH_B5_I8 },
  { "bbsi.l",0x006007, 0x700600, 24, XTENSA_OP_BRANCH_B5_I8 },
  { "beq",   0x001007, 0x700100, 24, XTENSA_OP_BRANCH_I8 },
  { "beqi",  0x000026, 0x620000, 24, XTENSA_OP_BRANCH_AS_C4_I8 },
  { "beqz",  0x000016, 0x610000, 24, XTENSA_OP_BRANCH_AS_I12 },
  { NULL, 0, 0, 0, 0 }
};

struct _mask_xtensa mask_xtensa[] =
{
  { 0xffffff, 0xffffff },  // XTENSA_OP_NONE
  { 0xff0f0f, 0xf0f0ff },  // XTENSA_OP_AR_AT
  { 0xff00ff, 0xff00ff },  // XTENSA_OP_FR_FS
  { 0xff000f, 0xf000ff },  // XTENSA_OP_FR_FS_FT
  { 0xff000f, 0xf000ff },  // XTENSA_OP_AR_AS_AT
  { 0x00f00f, 0xf00f00 },  // XTENSA_OP_AT_AS_I8
  { 0x00f00f, 0xf00f00 },  // XTENSA_OP_AT_AS_IM8
  { 0x00000f, 0x00f000 },  // XTENSA_OP_N_AR_AS_AT
  { 0x00000f, 0x00f000 },  // XTENSA_OP_N_AR_AS_I4
  { 0xfff00f, 0xf00fff },  // XTENSA_OP_BT_BS4
  { 0xfff00f, 0xf00fff },  // XTENSA_OP_BT_BS8
  { 0xff000f, 0xf000ff },  // XTENSA_OP_BR_BS_BT
  { 0x00f00f, 0xf00f00 },  // XTENSA_OP_BRANCH_I8
  { 0x00e00f, 0xf00e00 },  // XTENSA_OP_BRANCH_B5_I8
  { 0x0000ff, 0xff0000 },  // XTENSA_OP_BRANCH_AS_C4_I8
  { 0x0000ff, 0xff0000 },  // XTENSA_OP_BRANCH_AS_I12
};

int xtensa_b4const[] =
{
  -1,  1,  2,  3,  4,  5,   6,   7,
   8, 10, 12, 16, 32, 64, 128, 256
};

int xtensa_b4constu[] =
{
  32768, 65536,  2,  3,  4,  5,   6,   7,
      8,    10, 12, 16, 32, 64, 128, 256
};

