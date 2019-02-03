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
  { "abs",      0x600100, 0x001006, XTENSA_OP_AR_AT },
  { "abs.s",    0xfa0010, 0x0100af, XTENSA_OP_FR_FS },
  { "add",      0x800000, 0x000008, XTENSA_OP_AR_AS_AT },
  { "addi",     0x00c002, 0x200c00, XTENSA_OP_AT_AS_N128_127 },
  { "add.n",    0x00000a, 0x00a000, XTENSA_OP_N_AR_AS_AT },
  { "add.s",    0x0a0000, 0x0000a0, XTENSA_OP_FR_FS_FT },
  { "addi.n",   0x00000b, 0x00b000, XTENSA_OP_N_AR_AS_N1_15 },
  { "addmi",    0x00d002, 0x200d00, XTENSA_OP_AT_AS_N32768_32512 },
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
  { "bbci",     0x006007, 0x700600, XTENSA_OP_BRANCH_AS_B5_I8 },
  { "bbci.l",   0x006007, 0x700600, XTENSA_OP_BRANCH_AS_B5_I8_L },
  { "bbs",      0x00d007, 0x700d00, XTENSA_OP_BRANCH_AS_AT_I8 },
  { "bbsi",     0x00e007, 0x700e00, XTENSA_OP_BRANCH_AS_B5_I8 },
  { "bbsi.l",   0x00e007, 0x700e00, XTENSA_OP_BRANCH_AS_B5_I8_L },
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
  { "break.n",  0x00f02d, 0x00d20f, XTENSA_OP_N_NUM },
  { "call0",    0x000005, 0x500000, XTENSA_OP_CALL_I18 },
  { "call4",    0x000015, 0x540000, XTENSA_OP_CALL_I18 },
  { "call8",    0x000025, 0x580000, XTENSA_OP_CALL_I18 },
  { "call12",   0x000035, 0x5c0000, XTENSA_OP_CALL_I18 },
  { "callx0",   0x0000c0, 0x030000, XTENSA_OP_AS },
  { "callx4",   0x0000d0, 0x070000, XTENSA_OP_AS },
  { "callx8",   0x0000e0, 0x0b0000, XTENSA_OP_AS },
  { "callx12",  0x0000f0, 0x0f0000, XTENSA_OP_AS },
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
  { "extui",    0x040000, 0x000040, XTENSA_OP_AR_AT_SHIFT_MASK },
  { "extw",     0x0020d0, 0x0d0200, XTENSA_OP_NONE },
  { "float.s",  0xca0000, 0x0000ac, XTENSA_OP_FR_AS_0_15 },
  { "floor.s",  0xaa0000, 0x0000aa, XTENSA_OP_AR_FS_0_15 },
  { "idtlb",    0x50c000, 0x000c05, XTENSA_OP_AS },
  { "ihi",      0x0070e2, 0x2e0700, XTENSA_OP_AS_0_1020 },
  { "ihu",      0x0270B2, 0x2B0720, XTENSA_OP_AS_0_240 },
  { "iii",      0x0070f2, 0x2f0700, XTENSA_OP_AS_0_1020 },
  { "iitlb",    0x504000, 0x000405, XTENSA_OP_AS },
  { "iiu",      0x0370d2, 0x2d0730, XTENSA_OP_AS_0_240 },
  { "ill",      0x000000, 0x000000, XTENSA_OP_NONE },
  { "ill.n",    0x00f06d, 0x00d60f, XTENSA_OP_N_NONE },
  { "ipf",      0x0070c2, 0x2c0700, XTENSA_OP_AS_0_1020 },
  { "ipfl",     0x0070d2, 0x2d0700, XTENSA_OP_AS_0_240 },
  { "isync",    0x002000, 0x000200, XTENSA_OP_NONE },
  { "j",        0x000006, 0x600000, XTENSA_OP_JUMP_I18 },
  { "jx",       0x0000a0, 0x0a0000, XTENSA_OP_AS },
  { "l8ui",     0x000002, 0x200000, XTENSA_OP_AT_AS_0_255 },
  { "l16si",    0x009002, 0x200900, XTENSA_OP_AT_AS_0_510 },
  { "l16ui",    0x001002, 0x200100, XTENSA_OP_AT_AS_0_510 },
  { "l32ai",    0x00b002, 0x200b00, XTENSA_OP_AT_AS_0_1020 },
  { "l32e",     0x090000, 0x090000, XTENSA_OP_AT_AS_N64_N4 },
  { "l32i",     0x002002, 0x200200, XTENSA_OP_AT_AS_0_1020 },
  { "l32i.n",   0x000008, 0x008000, XTENSA_OP_N_AT_AS_0_60 },
  { "l32r",     0x000001, 0x100000, XTENSA_OP_AT_I16 },
  { "ldct",     0xf18000, 0x00081f, XTENSA_OP_AT_AS },
  { "lddec",    0x900004, 0x400009, XTENSA_OP_MW_AS },
  { "ldinc",    0x800004, 0x400008, XTENSA_OP_MW_AS },
  { "lict",     0xf10000, 0x00001f, XTENSA_OP_AT_AS },
  { "licw",     0xf12000, 0x00021f, XTENSA_OP_AT_AS },
  { "loop",     0x008076, 0x670800, XTENSA_OP_LOOP_AS_LABEL },
  { "loopgtz",  0x00a076, 0x670a00, XTENSA_OP_LOOP_AS_LABEL },
  { "loopnez",  0x009076, 0x670900, XTENSA_OP_LOOP_AS_LABEL },
  { "lsi",      0x000003, 0x300000, XTENSA_OP_FT_AS_0_1020 },
  { "lsiu",     0x008003, 0x300800, XTENSA_OP_FT_AS_0_1020 },
  { "lsx",      0x080000, 0x000080, XTENSA_OP_FR_AS_AT },
  { "lsxu",     0x180000, 0x000081, XTENSA_OP_FR_AS_AT },
  { "madd.s",   0x4a0000, 0x0000a4, XTENSA_OP_FR_FS_FT },
  { "max",      0x530000, 0x000035, XTENSA_OP_AR_AS_AT },
  { "maxu",     0x730000, 0x000037, XTENSA_OP_AR_AS_AT },
  { "memw",     0x0020c0, 0x0c0200, XTENSA_OP_NONE },
  { "min",      0x430000, 0x000034, XTENSA_OP_AR_AS_AT },
  { "minu",     0x630000, 0x000036, XTENSA_OP_AR_AS_AT },
  { "mov.n",    0x00000d, 0x00d000, XTENSA_OP_N_AT_AS },
  { "mov.s",    0xfa0000, 0x0000af, XTENSA_OP_FR_FS },
  { "moveqz",   0x830000, 0x000038, XTENSA_OP_AR_AS_AT },
  { "moveqz.s", 0x8b0000, 0x0000b8, XTENSA_OP_FR_FS_AT },
  { "movf",     0xc30000, 0x00003c, XTENSA_OP_AR_AS_BT },
  { "movf.s",   0xcb0000, 0x0000bc, XTENSA_OP_FR_FS_BT },
  { "movgez",   0xb30000, 0x00003b, XTENSA_OP_AR_AS_AT },
  { "movgez.s", 0xbb0000, 0x0000bb, XTENSA_OP_FR_FS_AT },
  { "movi",     0x00a002, 0x200a00, XTENSA_OP_AT_N2048_2047 },
  { "movi.n",   0x00000c, 0x00c000, XTENSA_OP_N_AS_N2048_2047 },
  { "movltz",   0xa30000, 0x00003a, XTENSA_OP_AR_AS_AT },
  { "movltz.s", 0xab0000, 0x0000ba, XTENSA_OP_FR_FS_AT },
  { "movnez",   0x930000, 0x000039, XTENSA_OP_AR_AS_AT },
  { "movnez.s", 0x9b0000, 0x0000b9, XTENSA_OP_FR_FS_AT },
  { "movsp",    0x001000, 0x000100, XTENSA_OP_AT_AS },
  { "movt",     0xd30000, 0x00003d, XTENSA_OP_AR_AS_BT },
  { "movt.s",   0xdb0000, 0x0000bd, XTENSA_OP_FR_FS_BT },
  { "msub.s",   0x5a0000, 0x0000a5, XTENSA_OP_FR_FS_FT },
  { "mul.aa.ll",0x740004, 0x400047, XTENSA_OP_AS_AT },
  { "mul.aa.hl",0x750004, 0x400057, XTENSA_OP_AS_AT },
  { "mul.aa.lh",0x760004, 0x400067, XTENSA_OP_AS_AT },
  { "mul.aa.hh",0x770004, 0x400077, XTENSA_OP_AS_AT },
  { "mul.ad.ll",0x340004, 0x400043, XTENSA_OP_AS_MY },
  { "mul.ad.hl",0x350004, 0x400053, XTENSA_OP_AS_MY },
  { "mul.ad.lh",0x360004, 0x400063, XTENSA_OP_AS_MY },
  { "mul.ad.hh",0x370004, 0x400073, XTENSA_OP_AS_MY },
  { "mul.da.ll",0x640004, 0x400046, XTENSA_OP_MX_AT },
  { "mul.da.hl",0x650004, 0x400056, XTENSA_OP_MX_AT },
  { "mul.da.lh",0x660004, 0x400066, XTENSA_OP_MX_AT },
  { "mul.da.hh",0x670004, 0x400076, XTENSA_OP_MX_AT },
  { "mul.dd.ll",0x240004, 0x400042, XTENSA_OP_MX_MY },
  { "mul.dd.hl",0x250004, 0x400052, XTENSA_OP_MX_MY },
  { "mul.dd.lh",0x260004, 0x400062, XTENSA_OP_MX_MY },
  { "mul.dd.hh",0x270004, 0x400072, XTENSA_OP_MX_MY },
  { "mul.s",    0x2a0000, 0x0000a2, XTENSA_OP_FR_FS_FT },
  { "mul16s",   0xd10000, 0x00001d, XTENSA_OP_AR_AS_AT },
  { "mul16u",   0xc10000, 0x00001c, XTENSA_OP_AR_AS_AT },
  { "mula.aa.ll",       0x780004, 0x400087, XTENSA_OP_AS_AT },
  { "mula.aa.hl",       0x790004, 0x400097, XTENSA_OP_AS_AT },
  { "mula.aa.lh",       0x7a0004, 0x4000a7, XTENSA_OP_AS_AT },
  { "mula.aa.hh",       0x7b0004, 0x4000b7, XTENSA_OP_AS_AT },
  { "mula.ad.ll",       0x380004, 0x400083, XTENSA_OP_AS_MY },
  { "mula.ad.hl",       0x390004, 0x400093, XTENSA_OP_AS_MY },
  { "mula.ad.lh",       0x3a0004, 0x4000a3, XTENSA_OP_AS_MY },
  { "mula.ad.hh",       0x3b0004, 0x4000b3, XTENSA_OP_AS_MY },
  { "mula.da.ll",       0x680004, 0x400086, XTENSA_OP_MX_AT },
  { "mula.da.hl",       0x690004, 0x400096, XTENSA_OP_MX_AT },
  { "mula.da.lh",       0x6a0004, 0x4000a6, XTENSA_OP_MX_AT },
  { "mula.da.hh",       0x6b0004, 0x4000b6, XTENSA_OP_MX_AT },
  { "mula.da.ll.lddec", 0x580004, 0x400085, XTENSA_OP_MW_AS_MX_AT },
  { "mula.da.hl.lddec", 0x590004, 0x400095, XTENSA_OP_MW_AS_MX_AT },
  { "mula.da.lh.lddec", 0x5a0004, 0x4000a5, XTENSA_OP_MW_AS_MX_AT },
  { "mula.da.hh.lddec", 0x5b0004, 0x4000b5, XTENSA_OP_MW_AS_MX_AT },
  { "mula.da.ll.ldinc", 0x480004, 0x400084, XTENSA_OP_MW_AS_MX_AT },
  { "mula.da.hl.ldinc", 0x490004, 0x400094, XTENSA_OP_MW_AS_MX_AT },
  { "mula.da.lh.ldinc", 0x4a0004, 0x4000a4, XTENSA_OP_MW_AS_MX_AT },
  { "mula.da.hh.ldinc", 0x4b0004, 0x4000b4, XTENSA_OP_MW_AS_MX_AT },
  { "mula.dd.ll",       0x280004, 0x400082, XTENSA_OP_MX_MY },
  { "mula.dd.hl",       0x290004, 0x400092, XTENSA_OP_MX_MY },
  { "mula.dd.lh",       0x2a0004, 0x4000a2, XTENSA_OP_MX_MY },
  { "mula.dd.hh",       0x2b0004, 0x4000b2, XTENSA_OP_MX_MY },
  { "mula.dd.ll.lddec", 0x180004, 0x400081, XTENSA_OP_MW_AS_MX_MY },
  { "mula.dd.hl.lddec", 0x190004, 0x400091, XTENSA_OP_MW_AS_MX_MY },
  { "mula.dd.lh.lddec", 0x1a0004, 0x4000a1, XTENSA_OP_MW_AS_MX_MY },
  { "mula.dd.hh.lddec", 0x1b0004, 0x4000b1, XTENSA_OP_MW_AS_MX_MY },
  { "mula.dd.ll.ldinc", 0x080004, 0x400080, XTENSA_OP_MW_AS_MX_MY },
  { "mula.dd.hl.ldinc", 0x090004, 0x400090, XTENSA_OP_MW_AS_MX_MY },
  { "mula.dd.lh.ldinc", 0x0a0004, 0x4000a0, XTENSA_OP_MW_AS_MX_MY },
  { "mula.dd.hh.ldinc", 0x0b0004, 0x4000b0, XTENSA_OP_MW_AS_MX_MY },
  { "mull",             0x820000, 0x000028, XTENSA_OP_AR_AS_AT },
  { "muls.aa.ll",       0x7c0004, 0x4000c7, XTENSA_OP_AS_AT },
  { "muls.aa.hl",       0x7d0004, 0x4000d7, XTENSA_OP_AS_AT },
  { "mula.aa.lh",       0x7e0004, 0x4000e7, XTENSA_OP_AS_AT },
  { "muls.aa.hh",       0x7f0004, 0x4000f7, XTENSA_OP_AS_AT },
  { "muls.ad.ll",       0x3c0004, 0x4000c3, XTENSA_OP_AS_MY },
  { "muls.ad.hl",       0x3d0004, 0x4000d3, XTENSA_OP_AS_MY },
  { "muls.ad.lh",       0x3e0004, 0x4000e3, XTENSA_OP_AS_MY },
  { "muls.ad.hh",       0x3f0004, 0x4000f3, XTENSA_OP_AS_MY },
  { "muls.da.ll",       0x6c0004, 0x4000c6, XTENSA_OP_MX_AT },
  { "muls.da.hl",       0x6d0004, 0x4000d6, XTENSA_OP_MX_AT },
  { "muls.da.lh",       0x6e0004, 0x4000e6, XTENSA_OP_MX_AT },
  { "muls.da.hh",       0x6f0004, 0x4000f6, XTENSA_OP_MX_AT },
  { "muls.dd.ll",       0x2c0004, 0x4000c2, XTENSA_OP_MX_MY },
  { "muls.dd.hl",       0x2d0004, 0x4000d2, XTENSA_OP_MX_MY },
  { "muls.dd.lh",       0x2e0004, 0x4000e2, XTENSA_OP_MX_MY },
  { "muls.dd.hh",       0x2f0004, 0x4000f2, XTENSA_OP_MX_MY },
  { "mulsh",    0xb20000, 0x00002b, XTENSA_OP_AR_AS_AT },
  { "muluh",    0xa20000, 0x00002a, XTENSA_OP_AR_AS_AT },
  { "neg",      0x600000, 0x000006, XTENSA_OP_AR_AT },
  { "neg.s",    0xfa0060, 0x0600af, XTENSA_OP_FR_FS },
  { "nop",      0x0020f0, 0x0f0200, XTENSA_OP_NONE },
  { "nop.n",    0x00f03d, 0x00d30f, XTENSA_OP_N_NONE },
  { "nsa",      0x40e000, 0x000e04, XTENSA_OP_AT_AS },
  { "nsau",     0x40f000, 0x000f04, XTENSA_OP_AT_AS },
  { "oeq.s",    0x2b0000, 0x0000b2, XTENSA_OP_BR_FS_FT },
  { "ole.s",    0x6b0000, 0x0000b6, XTENSA_OP_BR_FS_FT },
  { "olt.s",    0x4b0000, 0x0000b4, XTENSA_OP_BR_FS_FT },
  { "or",       0x200000, 0x000002, XTENSA_OP_AR_AS_AT },
  { "orb",      0x220000, 0x000022, XTENSA_OP_BR_BS_BT },
  { "orbc",     0x320000, 0x000023, XTENSA_OP_BR_BS_BT },
  { "pdtlb",    0x50d000, 0x000d05, XTENSA_OP_AT_AS },
  { "pitlb",    0x505000, 0x000505, XTENSA_OP_AT_AS },
  { "quos",     0xd20000, 0x00002d, XTENSA_OP_AR_AS_AT },
  { "quou",     0xc20000, 0x00002c, XTENSA_OP_AR_AS_AT },
  { "rdtlb0",   0x50b000, 0x000b05, XTENSA_OP_AT_AS },
  { "rdtlb1",   0x50f000, 0x000f05, XTENSA_OP_AT_AS },
  { "rems",     0xf20000, 0x00002f, XTENSA_OP_AR_AS_AT },
  { "remu",     0xe20000, 0x00002e, XTENSA_OP_AR_AS_AT },
  { "rer",      0x406000, 0x000604, XTENSA_OP_AT_AS },
  { "ret",      0x000080, 0x020000, XTENSA_OP_NONE },
  { "ret.n",    0x00f00d, 0x00d00f, XTENSA_OP_N_NONE },
  { "retw",     0x000090, 0x060000, XTENSA_OP_NONE },
  { "retw.n",   0x00f01d, 0x00d10f, XTENSA_OP_N_NONE },
  { "rfdd",     0xf1e010, 0x010e1f, XTENSA_OP_NONE },
  { "rfde",     0x003200, 0x002300, XTENSA_OP_NONE },
  { "rfdo",     0xf1e000, 0x000e1f, XTENSA_OP_NONE },
  { "rfe",      0x003000, 0x000300, XTENSA_OP_NONE },
  { "rfi",      0x003010, 0x010300, XTENSA_OP_0_15 },
  { "rfme",     0x003020, 0x020300, XTENSA_OP_NONE },
  { "rfr",      0xfa0040, 0x0400af, XTENSA_OP_AR_FS },
  { "rfue",     0x003100, 0x001300, XTENSA_OP_NONE },
  { "rfwo",     0x003400, 0x004300, XTENSA_OP_NONE },
  { "rfwu",     0x003500, 0x005300, XTENSA_OP_NONE },
  { "ritlb0",   0x503000, 0x000305, XTENSA_OP_AT_AS },
  { "ritlb1",   0x507000, 0x000705, XTENSA_OP_AT_AS },
  { "rotw",     0x408000, 0x000804, XTENSA_OP_N8_7 },
  { "round.s",  0x8a0000, 0x0000a8, XTENSA_OP_AR_FS_0_15 },
  { "rsil",     0x006000, 0x000600, XTENSA_OP_AT_0_15 },
  { "rsr",      0x030000, 0x000030, XTENSA_OP_AT_SPR },
  { "rsync",    0x002010, 0x010200, XTENSA_OP_NONE },
  { "rur",      0xe30000, 0x00003e, XTENSA_OP_AR_UR },
  { "s8i",      0x004002, 0x200400, XTENSA_OP_AT_AS_0_255 },
  { "s16i",     0x005002, 0x200500, XTENSA_OP_AT_AS_0_510 },
  { "s32c1i",   0x00e002, 0x200e00, XTENSA_OP_AT_AS_0_1020 },
  { "s32e",     0x490000, 0x000094, XTENSA_OP_AT_AS_N64_N4 },
  { "s32i",     0x006002, 0x200600, XTENSA_OP_AT_AS_0_1020 },
  { "s32i.n",   0x000009, 0x009000, XTENSA_OP_N_AT_AS_0_60 },
  { "s32ri",    0x00f002, 0x200f00, XTENSA_OP_AT_AS_0_1020 },
  { "sdct",     0xf19000, 0x00091f, XTENSA_OP_AT_AS },
  { "sext",     0x230000, 0x000032, XTENSA_OP_AR_AS_7_22 },
  { "sict",     0xf11000, 0x00011f, XTENSA_OP_AT_AS },
  { "sicw",     0xf13000, 0x00031f, XTENSA_OP_AT_AS },
  { "simcall",  0x005100, 0x001500, XTENSA_OP_NONE },
  { "sll",      0xa10000, 0x00001a, XTENSA_OP_AR_AS },
  { "slli",     0x010000, 0x000010, XTENSA_OP_AR_AS_1_31 },
  { "sra",      0xb10000, 0x00001b, XTENSA_OP_AR_AT },
  { "srai",     0x210000, 0x000012, XTENSA_OP_AR_AT_1_31 },
  { "src",      0x810000, 0x000018, XTENSA_OP_AR_AS_AT },
  { "srl",      0x910000, 0x000019, XTENSA_OP_AR_AT },
  { "srli",     0x410000, 0x000014, XTENSA_OP_AR_AT_0_15 },
  { "ssa8b",    0x403000, 0x000304, XTENSA_OP_AS },
  { "ssa8l",    0x402000, 0x000204, XTENSA_OP_AS },
  { "ssai",     0x404000, 0x000404, XTENSA_OP_0_31 },
  { "ssi",      0x004003, 0x300400, XTENSA_OP_FT_AS_0_1020 },
  { "ssiu",     0x00c003, 0x300c00, XTENSA_OP_FT_AS_0_1020 },
  { "ssl",      0x401000, 0x000104, XTENSA_OP_AS },
  { "ssr",      0x400000, 0x000004, XTENSA_OP_AS },
  { "ssx",      0x480000, 0x000084, XTENSA_OP_FR_AS_AT },
  { "ssxu",     0x580000, 0x000085, XTENSA_OP_FR_AS_AT },
  { "sub",      0xc00000, 0x00000c, XTENSA_OP_AR_AS_AT },
  { "sub.s",    0x1a0000, 0x0000a1, XTENSA_OP_FR_FS_FT },
  { "subx2",    0xd00000, 0x00000d, XTENSA_OP_AR_AS_AT },
  { "subx4",    0xe00000, 0x00000e, XTENSA_OP_AR_AS_AT },
  { "subx8",    0xf00000, 0x00000f, XTENSA_OP_AR_AS_AT },
  { "syscall",  0x005000, 0x000500, XTENSA_OP_NONE },
  { "trunc.s",  0x9a0000, 0x0000a9, XTENSA_OP_AR_FS_0_15 },
  { "ueq.s",    0x3b0000, 0x0000b3, XTENSA_OP_BR_FS_FT },
  { "ufloat.s", 0xda0000, 0x0000ad, XTENSA_OP_FR_AS_0_15 },
  { "ule.s",    0x7b0000, 0x0000b7, XTENSA_OP_BR_FS_FT },
  { "ult.s",    0x5b0000, 0x0000b5, XTENSA_OP_BR_FS_FT },
  { "umul.aa.ll", 0x700004, 0x400007, XTENSA_OP_AS_AT },
  { "umul.aa.hl", 0x710004, 0x400017, XTENSA_OP_AS_AT },
  { "umul.aa.lh", 0x720004, 0x400027, XTENSA_OP_AS_AT },
  { "umul.aa.hh", 0x730004, 0x400037, XTENSA_OP_AS_AT },
  { "un.s",     0x1b0000, 0x0000b1, XTENSA_OP_BR_FS_FT },
  { "utrunc.s", 0xea0000, 0x0000ae, XTENSA_OP_AR_FS_0_15 },
  { "waiti",    0x007000, 0x000700, XTENSA_OP_0_15 },
  { "wdtlb",    0x50e000, 0x000e05, XTENSA_OP_AT_AS },
  { "wer",      0x407000, 0x000704, XTENSA_OP_AT_AS },
  { "wfr",      0xfa0050, 0x0500af, XTENSA_OP_FR_AS },
  { "witlb",    0x506000, 0x000605, XTENSA_OP_AT_AS },
  { "wsr",      0x130000, 0x000031, XTENSA_OP_AT_SPR },
  { "wur",      0xf30000, 0x00003f, XTENSA_OP_AT_SPR },
  { "xor",      0x300000, 0x000003, XTENSA_OP_AR_AS_AT },
  { "xorb",     0x420000, 0x000024, XTENSA_OP_BR_BS_BT },
  { "xsr",      0x610000, 0x000016, XTENSA_OP_AT_SPR },
  { "mov",      0x200000, 0x000002, XTENSA_OP_AR_AS_AS },
  { NULL, 0, 0, 0 }
};

enum
{
  AR = 1,
  FR = 2,
  BR = 3,
  MW = 4,
};

struct _mask_xtensa mask_xtensa[] =
{
  { 0xffffff, 0xffffff, 24,  0,  0,  0 },  // XTENSA_OP_NONE
  { 0x00ffff, 0x00ffff, 16,  0,  0,  0 },  // XTENSA_OP_N_NONE
  { 0xff0f0f, 0xf0f0ff, 24, AR, AR,  0 },  // XTENSA_OP_AR_AT
  { 0xfff00f, 0xf00fff, 24, AR, AR,  0 },  // XTENSA_OP_AT_AS
  { 0xff00ff, 0xff00ff, 24, FR, FR,  0 },  // XTENSA_OP_FR_FS
  { 0xff000f, 0xf000ff, 24, FR, FR, FR },  // XTENSA_OP_FR_FS_FT
  { 0xff000f, 0xf000ff, 24, AR, AR, AR },  // XTENSA_OP_AR_AS_AT
  { 0x00f00f, 0xf00f00, 24, AR, AR,  0 },  // XTENSA_OP_AT_AS_N128_127
  { 0x00f00f, 0xf00f00, 24, AR, AR,  0 },  // XTENSA_OP_AT_AS_N32768_32512
  { 0x00000f, 0x00f000, 16, AR, AR, AR },  // XTENSA_OP_N_AR_AS_AT
  { 0x00000f, 0x00f000, 16, AR, AR,  0 },  // XTENSA_OP_N_AR_AS_N1_15
  { 0xfff00f, 0xf00fff, 24, BR,  0,  0 },  // XTENSA_OP_BT_BS4
  { 0xfff00f, 0xf00fff, 24, BR,  0,  0 },  // XTENSA_OP_BT_BS8
  { 0xff000f, 0xf000ff, 24, BR, BR, BR },  // XTENSA_OP_BR_BS_BT
  { 0x00f00f, 0xf00f00, 24, AR, AR,  0 },  // XTENSA_OP_BRANCH_AS_AT_I8
  { 0x00e00f, 0xf00e00, 24,  0,  0,  0 },  // XTENSA_OP_BRANCH_AS_B5_I8
  { 0x00e00f, 0xf00e00, 24,  0,  0,  0 },  // XTENSA_OP_BRANCH_AS_B5_I8_L
  { 0x0000ff, 0xff0000, 24, AR,  0,  0 },  // XTENSA_OP_BRANCH_AS_C4_I8
  { 0x0000ff, 0xff0000, 24, AR,  0,  0 },  // XTENSA_OP_BRANCH_AS_I12
  { 0x0000cf, 0x00fc00, 16, AR,  0,  0 },  // XTENSA_OP_BRANCH_N_AS_I6
  { 0x00f0ff, 0xff0f00, 24,  0,  0,  0 },  // XTENSA_OP_BRANCH_BS_I8
  { 0xfff00f, 0xf00fff, 24,  0,  0,  0 },  // XTENSA_OP_NUM_NUM
  { 0x00f0ff, 0x00ff0f, 16,  0,  0,  0 },  // XTENSA_OP_N_NUM
  { 0x00003f, 0xfc0000, 24,  0,  0,  0 },  // XTENSA_OP_CALL_I18
  { 0x00003f, 0xfc0000, 24,  0,  0,  0 },  // XTENSA_OP_JUMP_I18
  { 0xfff0ff, 0xff0fff, 24, AR,  0,  0 },  // XTENSA_OP_AS
  { 0xff000f, 0xf000ff, 24, AR, FR,  0 },  // XTENSA_OP_AR_FS_0_15
  { 0xff000f, 0xf000ff, 24, AR, AR,  0 },  // XTENSA_OP_AR_AS_7_22
  { 0x00f0ff, 0xff0f00, 24, AR,  0,  0 },  // XTENSA_OP_AS_0_1020
  { 0x0ff0ff, 0xff0ff0, 24, AR,  0,  0 },  // XTENSA_OP_AS_0_240
  { 0x0000ff, 0xff0000, 24, AR,  0,  0 },  // XTENSA_OP_AS_0_32760
  { 0x0e000f, 0xf000e0, 24, AR, AR,  0 },  // XTENSA_OP_AR_AT_SHIFT_MASK
  { 0xff000f, 0xf000ff, 24, FR, AR,  0 },  // XTENSA_OP_FR_AS_0_15
  { 0x00f00f, 0xf00f00, 24, AR, AR,  0 },  // XTENSA_OP_AT_AS_0_255
  { 0x00f00f, 0xf00f00, 24, AR, AR,  0 },  // XTENSA_OP_AT_AS_0_510
  { 0x00f00f, 0xf00f00, 24, AR, AR,  0 },  // XTENSA_OP_AT_AS_0_1020
  { 0xff000f, 0xf000ff, 24, AR, AR,  0 },  // XTENSA_OP_AT_AS_N64_N4
  { 0x00000f, 0xf0f000, 16, AR, AR,  0 },  // XTENSA_OP_N_AT_AS_0_60
  { 0x00000f, 0xf00000, 24, AR,  0,  0 },  // XTENSA_OP_AT_I16
  { 0xffc0ff, 0xff0cff, 24,  0,  0,  0 },  // XTENSA_OP_MW_AS
  { 0x00f0ff, 0xff0f00, 24,  0,  0,  0 },  // XTENSA_OP_LOOP_AS_LABEL
  { 0x00f00f, 0xf00f00, 24, FR, AR,  0 },  // XTENSA_OP_FT_AS_0_1020
  { 0xff000f, 0xf000ff, 24, FR, AR, AR },  // XTENSA_OP_FR_AS_AT
  { 0xff000f, 0xf000ff, 24, AR, AR, AR },  // XTENSA_OP_AR_AS_AS
  { 0x00f00f, 0x00f00f, 16, AR, AR,  0 },  // XTENSA_OP_N_AT_AS
  { 0xff000f, 0xf000ff, 24, FR, FR, AR },  // XTENSA_OP_FR_FS_AT
  { 0xff000f, 0xf000ff, 24, AR, AR, BR },  // XTENSA_OP_AR_AS_BT
  { 0xff000f, 0xf000ff, 24, FR, FR, BR },  // XTENSA_OP_FR_FS_BT
  { 0x00f00f, 0x00f00f, 24, AR,  0,  0 },  // XTENSA_OP_AT_N2048_2047
  { 0x00008f, 0x00f800, 16, AR,  0,  0 },  // XTENSA_OP_N_AS_N2048_2047
  { 0xfff00f, 0xf00fff, 24, AR, AR,  0 },  // XTENSA_OP_AS_AT
  { 0xfff0bf, 0xfb0fff, 24, AR, MW,  0 },  // XTENSA_OP_AS_MY
  { 0xffbf0f, 0xf0fbff, 24, MW, AR,  0 },  // XTENSA_OP_MX_AT
  { 0xffb0bf, 0xfb0bff, 24, MW, MW,  0 },  // XTENSA_OP_MX_MY
  { 0xfc800f, 0xf008cf, 24, MW, AR, MW },  // XTENSA_OP_MW_AS_MX_AT
  { 0xfc80bf, 0xfb08cf, 24, MW, AR, MW },  // XTENSA_OP_MW_AS_MX_MY
  { 0xff000f, 0xf000ff, 24, BR, FR, FR },  // XTENSA_OP_BR_FS_FT
  { 0xfff0ff, 0xff0fff, 24,  0,  0,  0 },  // XTENSA_OP_0_15
  { 0xff00ff, 0xff00ff, 24, AR, FR,  0 },  // XTENSA_OP_AR_FS
  { 0xffff0f, 0xf0ffff, 24,  0,  0,  0 },  // XTENSA_OP_N8_7
  { 0xfff00f, 0xf00fff, 24, AR,  0,  0 },  // XTENSA_OP_AT_0_15
  { 0xff000f, 0xf000ff, 24, AR,  0,  0 },  // XTENSA_OP_AT_SPR
  { 0xff000f, 0xf000ff, 24, AR,  0,  0 },  // XTENSA_OP_AR_UR
  { 0xff00ff, 0xff00ff, 24, AR,  AR, 0 },  // XTENSA_OP_AR_AS
  { 0xef000f, 0xf000fe, 24, AR,  AR, 0 },  // XTENSA_OP_AR_AS_1_31
  { 0xef000f, 0xf000fe, 24, AR,  AR, 0 },  // XTENSA_OP_AR_AT_1_31
  { 0xff000f, 0xf000ff, 24, AR,  AR, 0 },  // XTENSA_OP_AR_AT_0_15
  { 0xfff0ef, 0xfe0fff, 24,  0,   0, 0 },  // XTENSA_OP_0_31
  { 0xff00ff, 0xff00ff, 24, FR,  AR, 0 },  // XTENSA_OP_FR_AS
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

