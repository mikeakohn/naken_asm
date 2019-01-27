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
  { "abs",      0x060100, 0x001006, XTENSA_OP_AR_AT },
  { "abs.s",    0xfa0010, 0x0100af, XTENSA_OP_FR_FS },
  { "add",      0x800000, 0x000008, XTENSA_OP_AR_AS_AT },
  { "addi",     0x00c002, 0x200c00, XTENSA_OP_AT_AS_I8 },
  { "add.n",    0x00000a, 0x00a000, XTENSA_OP_N_AR_AS_AT },
  { "add.s",    0x0a0000, 0x0000a0, XTENSA_OP_FR_FS_FT },
  { "addi.n",   0x00000b, 0x00b000, XTENSA_OP_N_AR_AS_I4 },
  { "addmi",    0x00d002, 0x200d00, XTENSA_OP_AT_AS_IM8 },
  { "addx2",    0x900000, 0x000009, XTENSA_OP_AR_AS_AT },
  { "addx4",    0xa00000, 0x00000a, XTENSA_OP_AR_AS_AT },
  { "addx8",    0xb00000, 0x00000b, XTENSA_OP_AR_AS_AT },
  { "all4",     0x009000, 0x000900, XTENSA_OP_BT_BS4 },
  { "all8",     0x00b000, 0x000b00, XTENSA_OP_BT_BS8 },
  { "and",      0x100000, 0x000001, XTENSA_OP_AR_AS_AT },
  { "andb",     0x020000, 0x000020, XTENSA_OP_BR_BS_BT },
  { "andbc",    0x120000, 0x000021, XTENSA_OP_BR_BS_BT },
  { "any4",     0x008000, 0x000800, XTENSA_OP_BT_BS4 },
  { "any8",     0x00a000, 0x000a00, XTENSA_OP_BT_BS8 },
  { "ball",     0x004007, 0x700400, XTENSA_OP_BRANCH_AS_AT_I8 },
  { "bany",     0x008007, 0x700800, XTENSA_OP_BRANCH_AS_AT_I8 },
  { "bbc",      0x005007, 0x700500, XTENSA_OP_BRANCH_AS_AT_I8 },
  { "bbci",     0x006007, 0x700600, XTENSA_OP_BRANCH_B5_I8 },
  { "bbci.l",   0x006007, 0x700600, XTENSA_OP_BRANCH_B5_I8 },
  { "bbs",      0x00d007, 0x700d00, XTENSA_OP_BRANCH_AS_AT_I8 },
  { "bbsi",     0x00e007, 0x700e00, XTENSA_OP_BRANCH_B5_I8 },
  { "bbsi.l",   0x006007, 0x700600, XTENSA_OP_BRANCH_B5_I8 },
  { "beq",      0x001007, 0x700100, XTENSA_OP_BRANCH_AS_AT_I8 },
  { "beqi",     0x000026, 0x620000, XTENSA_OP_BRANCH_AS_C4_I8 },
  { "beqz",     0x000016, 0x610000, XTENSA_OP_BRANCH_AS_I12 },
  { "beqz.n",   0x00008c, 0x00c800, XTENSA_OP_BRANCH_N_AS_I6 },
  { "bf",       0x000076, 0x670000, XTENSA_OP_BRANCH_BS_I8 },
  { "bge",      0x00a007, 0x700a00, XTENSA_OP_BRANCH_AS_AT_I8 },
  { "bgei",     0x0000e6, 0x6e0000, XTENSA_OP_BRANCH_AS_C4_I8 },
  { "bgeu",     0x00b007, 0x700b00, XTENSA_OP_BRANCH_AS_AT_I8 },
  { "bgeui",    0x0000f6, 0x6f0000, XTENSA_OP_BRANCH_AS_C4_I8 },
  { "bgez",     0x0000d6, 0x6d0000, XTENSA_OP_BRANCH_AS_I12 },
  { "blt",      0x002007, 0x700200, XTENSA_OP_BRANCH_AS_AT_I8 },
  { "blti",     0x0000a6, 0x6a0000, XTENSA_OP_BRANCH_AS_C4_I8 },
  { "bltu",     0x003007, 0x700300, XTENSA_OP_BRANCH_AS_AT_I8 },
  { "bltui",    0x0000b6, 0x6b0000, XTENSA_OP_BRANCH_AS_C4_I8 },
  { "bltz",     0x000096, 0x690000, XTENSA_OP_BRANCH_AS_I12 },
  { "bnall",    0x00c007, 0x700c00, XTENSA_OP_BRANCH_AS_AT_I8 },
  { "bne",      0x009007, 0x700900, XTENSA_OP_BRANCH_AS_AT_I8 },
  { "bnei",     0x000066, 0x660000, XTENSA_OP_BRANCH_AS_C4_I8 },
  { "bnez",     0x000056, 0x650000, XTENSA_OP_BRANCH_AS_I12 },
  { "bnez.n",   0x0000cc, 0x00cc00, XTENSA_OP_BRANCH_N_AS_I6 },
  { "bnone",    0x000007, 0x700000, XTENSA_OP_BRANCH_AS_AT_I8 },
  { "break",    0x004000, 0x000400, XTENSA_OP_NUM_NUM },
  { "break.n",  0x00f02d, 0x00d20f, XTENSA_OP_N_NUM_NUM },
  { "call0",    0x000005, 0x500000, XTENSA_OP_CALL_I18 },
  { "call4",    0x000015, 0x540000, XTENSA_OP_CALL_I18 },
  { "call8",    0x000025, 0x580000, XTENSA_OP_CALL_I18 },
  { "call12",   0x000035, 0x5c0000, XTENSA_OP_CALL_I18 },
  { "callx0",   0x0000c0, 0x030000, XTENSA_OP_CALL_AS },
  { "callx4",   0x0000d0, 0x070000, XTENSA_OP_CALL_AS },
  { "callx8",   0x0000e0, 0x0b0000, XTENSA_OP_CALL_AS },
  { "callx12",  0x0000f0, 0x0f0000, XTENSA_OP_CALL_AS },
  { "ceil.s",   0xba0000, 0x0000ab, XTENSA_OP_AR_FS_0_15 },
  { "clamps",   0x330000, 0x000033, XTENSA_OP_AR_AS_7_22 },
  { "dhi",      0x007062, 0x260700, XTENSA_OP_AS_0_1020 },
  { "dhu",      0x027082, 0x280720, XTENSA_OP_AS_0_240 },
  { "dhwb",     0x007042, 0x240700, XTENSA_OP_AS_0_1020 },
  { "dhwbi",    0x007052, 0x250700, XTENSA_OP_AS_0_1020 },
  { "dii",      0x007072, 0x270700, XTENSA_OP_AS_0_1020 },
  { "diu",      0x037082, 0x280730, XTENSA_OP_AS_0_240 },
  { "diwb",     0x047082, 0x280740, XTENSA_OP_AS_0_240 },
  { "diwbi",    0x057082, 0x280750, XTENSA_OP_AS_0_240 },
  { "dpfl",     0x007082, 0x280700, XTENSA_OP_AS_0_240 },
  { "dpfr",     0x007002, 0x200700, XTENSA_OP_AS_0_1020 },
  { "dpfro",    0x007022, 0x220700, XTENSA_OP_AS_0_1020 },
  { "dpfw",     0x007012, 0x210700, XTENSA_OP_AS_0_1020 },
  { "dpfwo",    0x007032, 0x230700, XTENSA_OP_AS_0_1020 },
  { "dsync",    0x002030, 0x030200, XTENSA_OP_NONE },
  { "entry",    0x000036, 0x6c0000, XTENSA_OP_AS_0_32760 },
  { "esync",    0x002020, 0x020200, XTENSA_OP_NONE },
  { "excw",     0x002080, 0x080200, XTENSA_OP_NONE },
  { "extui",    0x000000, 0x000000, XTENSA_OP_AR_AT_SHIFT_MASK },
  { "extw",     0x0020d0, 0x0d0200, XTENSA_OP_NONE },
  { "float.s",  0xca0000, 0x0000ac, XTENSA_OP_FR_AS_0_15 },
  { "float.s",  0xaa0000, 0x0000aa, XTENSA_OP_AR_FS_0_15 },
  { "idtlb",    0x50c000, 0x000c05, XTENSA_OP_CALL_AS },
  { "ihi",      0x0070e2, 0x2e0700, XTENSA_OP_AS_0_1020 },
  { "ihu",      0x0270B2, 0x2B0720, XTENSA_OP_AS_0_240 },
  { "iii",      0x0070f2, 0x2f0700, XTENSA_OP_AS_0_1020 },
  { "iitlb",    0x504000, 0x000405, XTENSA_OP_CALL_AS },
  { "iiu",      0x0370d2, 0x2d0730, XTENSA_OP_AS_0_240 },
  { "ill",      0x000000, 0x000000, XTENSA_OP_NONE },
  { "ill.n",    0x00f06d, 0x00d60f, XTENSA_OP_N_NONE },
  { "ipf",      0x0070c2, 0x2c0700, XTENSA_OP_AS_0_1020 },
  { "ipfl",     0x0070d2, 0x2d0700, XTENSA_OP_AS_0_240 },
  { "isync",    0x002000, 0x000200, XTENSA_OP_NONE },
  { "j",        0x000006, 0x600000, XTENSA_OP_CALL_I18 },
  { "jx",       0x0000a0, 0x0a0000, XTENSA_OP_CALL_AS },
  { "l8ui",     0x000002, 0x200000, XTENSA_OP_AT_AS_0_255 },
  { "l16si",    0x009002, 0x200900, XTENSA_OP_AT_AS_0_510 },
  { "l16ui",    0x001002, 0x200100, XTENSA_OP_AT_AS_0_510 },
  { "l32ai",    0x00b002, 0x200b00, XTENSA_OP_AT_AS_0_1020 },
  { "l32e",     0x090000, 0x090000, XTENSA_OP_AT_AS_N64_N4 },
  { NULL, 0, 0, 0 }
};

struct _mask_xtensa mask_xtensa[] =
{
  { 0xffffff, 0xffffff, 24 },  // XTENSA_OP_NONE
  { 0x00ffff, 0x00ffff, 16 },  // XTENSA_OP_N_NONE
  { 0xff0f0f, 0xf0f0ff, 24 },  // XTENSA_OP_AR_AT
  { 0xff00ff, 0xff00ff, 24 },  // XTENSA_OP_FR_FS
  { 0xff000f, 0xf000ff, 24 },  // XTENSA_OP_FR_FS_FT
  { 0xff000f, 0xf000ff, 24 },  // XTENSA_OP_AR_AS_AT
  { 0x00f00f, 0xf00f00, 24 },  // XTENSA_OP_AT_AS_I8
  { 0x00f00f, 0xf00f00, 24 },  // XTENSA_OP_AT_AS_IM8
  { 0x00000f, 0x00f000, 16 },  // XTENSA_OP_N_AR_AS_AT
  { 0x00000f, 0x00f000, 16 },  // XTENSA_OP_N_AR_AS_I4
  { 0xfff00f, 0xf00fff, 24 },  // XTENSA_OP_BT_BS4
  { 0xfff00f, 0xf00fff, 24 },  // XTENSA_OP_BT_BS8
  { 0xff000f, 0xf000ff, 24 },  // XTENSA_OP_BR_BS_BT
  { 0x00f00f, 0xf00f00, 24 },  // XTENSA_OP_BRANCH_AS_AT_I8
  { 0x00e00f, 0xf00e00, 24 },  // XTENSA_OP_BRANCH_B5_I8
  { 0x0000ff, 0xff0000, 24 },  // XTENSA_OP_BRANCH_AS_C4_I8
  { 0x0000ff, 0xff0000, 24 },  // XTENSA_OP_BRANCH_AS_I12
  { 0x0000cf, 0x00fc00, 16 },  // XTENSA_OP_BRANCH_N_AS_I6
  { 0x00f0ff, 0xff0f00, 24 },  // XTENSA_OP_BRANCH_BS_I8
  { 0xfff00f, 0xf00fff, 24 },  // XTENSA_OP_NUM_NUM
  { 0x00f0ff, 0x00ff0f, 16 },  // XTENSA_OP_N_NUM_NUM
  { 0x00003f, 0xfc0000, 24 },  // XTENSA_OP_CALL_I18
  { 0xfff0ff, 0xff0fff, 24 },  // XTENSA_OP_CALL_AS
  { 0xff000f, 0xf000ff, 24 },  // XTENSA_OP_AR_FS_0_15
  { 0xff000f, 0xf000ff, 24 },  // XTENSA_OP_AR_AS_7_22
  { 0x00f0ff, 0xff0f00, 24 },  // XTENSA_OP_AS_0_1020
  { 0x0ff0ff, 0xff0ff0, 24 },  // XTENSA_OP_AS_0_240
  { 0x0000ff, 0xff0000, 24 },  // XTENSA_OP_AS_0_32760
  { 0x0e000f, 0xf000e0, 24 },  // XTENSA_OP_AR_AT_SHIFT_MASK
  { 0xff000f, 0xf000ff, 24 },  // XTENSA_OP_FR_AS_0_15
  { 0x00f00f, 0xf00f00, 24 },  // XTENSA_OP_AT_AS_0_255
  { 0x00f00f, 0xf00f00, 24 },  // XTENSA_OP_AT_AS_0_510
  { 0x00f00f, 0xf00f00, 24 },  // XTENSA_OP_AT_AS_0_1020
  { 0xff000f, 0xf000ff, 24 },  // XTENSA_OP_AT_AS_N64_N4
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

