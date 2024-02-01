/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2024 by Michael Kohn
 *
 */

#include <stdlib.h>

#include "table/arm64.h"

struct _table_arm64 table_arm64[] =
{
  { "nop",       0xd503201f, 0xffffffff, 0, ' ', OP_NONE },
  { "abs",       0x5e20b800, 0xff3ffc00, 2, 'd', OP_SCALAR_D_D },
  { "abs",       0x0e20b800, 0xbf3ffc00, 2, 'v', OP_VECTOR_V_V },
  { "addg",      0x91800000, 0xffc0c000, 4, 'x', OP_MATH_R_R_IMM6_IMM4 },

  // Alias for subs / adds.
  { "cmp",       0x7100001f, 0x7f00001f, 2, 'b', OP_ALIAS_REG_IMM },
  { "cmn",       0x3100001f, 0x7f00001f, 2, 'b', OP_ALIAS_REG_IMM },

  { "addv",      0x0e31b800, 0xbf3ffc00, 2, 'd', OP_VECTOR_V_V_TO_SCALAR },

  { "shl",       0x5f005400, 0xff80fc00, 3, 'd', OP_SCALAR_SHIFT_IMM },
  { "shl",       0x0f005400, 0xbf80fc00, 3, 'd', OP_VECTOR_SHIFT_IMM },

  { "and",       0x0e201c00, 0xbfe0fc00, 3, 'v', OP_VECTOR_V_V_V },
  { "eor",       0x2e201c00, 0xbfe0fc00, 3, 'v', OP_VECTOR_V_V_V },
  { "orr",       0x0ea01c00, 0xbfe0fc00, 3, 'v', OP_VECTOR_V_V_V },

  { "add",       0x0e208400, 0xbf20fc00, 3, 'v', OP_VECTOR_V_V_V },
  { "sub",       0x2e208400, 0xbf20fc00, 3, 'v', OP_VECTOR_V_V_V },
  { "mul",       0x0e209c00, 0xbf20fc00, 3, 'v', OP_VECTOR_V_V_V },

  { "add",       0x5e208400, 0xff20fc00, 3, 'd', OP_SCALAR_D_D_D },
  { "sub",       0x7e208400, 0xff20fc00, 3, 'd', OP_SCALAR_D_D_D },

  { "fadd",      0x0e20d400, 0xbfa0fc00, 3, 'v', OP_VECTOR_V_V_V_FPU },
  { "fmax",      0x0e20f400, 0xbfa0fc00, 3, 'v', OP_VECTOR_V_V_V_FPU },
  { "fmin",      0x0ea0f400, 0xbfa0fc00, 3, 'v', OP_VECTOR_V_V_V_FPU },
  { "fmul",      0x2e20dc00, 0xbfa0fc00, 3, 'v', OP_VECTOR_V_V_V_FPU },
  { "fsub",      0x0ea0d400, 0xbfa0fc00, 3, 'v', OP_VECTOR_V_V_V_FPU },

  { "fneg",      0x2eaaf800, 0xbfbffc00, 3, 'v', OP_VECTOR_V_V_FPU },

  { "fadd",      0x1e202800, 0xff20fc00, 3, 'd', OP_VECTOR_D_D_D_FPU },
  { "fmax",      0x1e204800, 0xff20fc00, 3, 'd', OP_VECTOR_D_D_D_FPU },
  { "fmin",      0x1e205800, 0xff20fc00, 3, 'd', OP_VECTOR_D_D_D_FPU },
  { "fmul",      0x1e200800, 0xff20fc00, 3, 'd', OP_VECTOR_D_D_D_FPU },
  { "fsub",      0x1e203800, 0xff20fc00, 3, 'd', OP_VECTOR_D_D_D_FPU },

  { "fneg",      0x1e214000, 0xff3ffc00, 3, 'd', OP_VECTOR_D_D_FPU },

  { "cmeq",      0x2e208c00, 0xbf20fc00, 3, 'v', OP_VECTOR_V_V_V },
  { "cmge",      0x0e203c00, 0xbf20fc00, 3, 'v', OP_VECTOR_V_V_V },
  { "cmgt",      0x0e203400, 0xbf20fc00, 3, 'v', OP_VECTOR_V_V_V },
  { "cmhi",      0x2e203400, 0xbf20fc00, 3, 'v', OP_VECTOR_V_V_V },
  { "cmhs",      0x2e203c00, 0xbf20fc00, 3, 'v', OP_VECTOR_V_V_V },

  { "cmeq",      0x7e208c00, 0xff20fc00, 3, 'd', OP_SCALAR_D_D_D },
  { "cmge",      0x5e203c00, 0xff20fc00, 3, 'd', OP_SCALAR_D_D_D },
  { "cmgt",      0x5e203400, 0xff20fc00, 3, 'd', OP_SCALAR_D_D_D },
  { "cmhi",      0x7e203400, 0xff20fc00, 3, 'd', OP_SCALAR_D_D_D },
  { "cmhs",      0x7e203c00, 0xff20fc00, 3, 'd', OP_SCALAR_D_D_D },

  { "cmeq",      0x0e208800, 0xbf3ffc00, 2, 'v', OP_VECTOR_V_V },
  { "cmge",      0x2e208800, 0xbf3ffc00, 2, 'v', OP_VECTOR_V_V },
  { "cmgt",      0x0e208800, 0xbf3ffc00, 2, 'v', OP_VECTOR_V_V },
  { "cmle",      0x2e208800, 0xbf3ffc00, 2, 'v', OP_VECTOR_V_V },
  { "cmlt",      0x0e20a800, 0xbf3ffc00, 2, 'v', OP_VECTOR_V_V },

  { "cmeq",      0x5e209800, 0xff3ffc00, 2, 'd', OP_SCALAR_D_D },
  { "cmge",      0x7e208800, 0xff3ffc00, 2, 'd', OP_SCALAR_D_D },
  { "cmgt",      0x5e208800, 0xff3ffc00, 2, 'd', OP_SCALAR_D_D },
  { "cmle",      0x7e209800, 0xff3ffc00, 2, 'd', OP_SCALAR_D_D },
  { "cmlt",      0x5e20a800, 0xff3ffc00, 2, 'd', OP_SCALAR_D_D },

  { "fcmeq",     0x0e20e400, 0xbfa0fc00, 3, 'v', OP_VECTOR_V_V_V_FPU },
  { "fcmge",     0x2e20e400, 0xbfa0fc00, 3, 'v', OP_VECTOR_V_V_V_FPU },
  { "fcmgt",     0x2ea0e400, 0xbfa0fc00, 3, 'v', OP_VECTOR_V_V_V_FPU },

  { "fcmeq",     0x5e20e400, 0xffa0fc00, 3, 'd', OP_VECTOR_D_D_D_FPU },
  { "fcmge",     0x7e20e400, 0xffa0fc00, 3, 'd', OP_VECTOR_D_D_D_FPU },
  { "fcmgt",     0x7ea0e400, 0xffa0fc00, 3, 'd', OP_VECTOR_D_D_D_FPU },

  //{ "saddlv",    0x0e303800, 0xbf3ffc00, 2, 'd', OP_VECTOR_V_TO_SCALAR },

  { "at",        0xd5807800, 0xfff8fe00, 2, 'b', OP_AT },
  { "bcax",      0xce200000, 0xffe08000, 4, 'v', OP_REG_REG_CRYPT },
  { "bfc",       0xce200000, 0x7f8003e0, 3, 'b', OP_REG_BITFIELD },

  // C2.4.6 Shift (register).
  { "lslv",      0x1ac02800, 0x7fe0fc00, 3, 'b', OP_MATH_R_R_R },
  { "lsrv",      0x1ac02400, 0x7fe0fc00, 3, 'b', OP_MATH_R_R_R },
  { "asrv",      0x1ac02800, 0x7fe0fc00, 3, 'b', OP_MATH_R_R_R },
  { "rorv",      0x1ac02c00, 0x7fe0fc00, 3, 'b', OP_MATH_R_R_R },

  { "lsl",       0x1ac02800, 0x7fe0fc00, 3, 'b', OP_MATH_R_R_R },
  { "lsr",       0x1ac02800, 0x7fe0fc00, 3, 'b', OP_MATH_R_R_R },
  { "asr",       0x1ac02800, 0x7fe0fc00, 3, 'b', OP_MATH_R_R_R },
  { "ror",       0x1ac02800, 0x7fe0fc00, 3, 'b', OP_MATH_R_R_R },

  { "lsl",       0x13007c00, 0x7f80fc00, 3, 'b', OP_MATH_R_R_IMMR },
  { "lsr",       0x13007c00, 0x7f80fc00, 3, 'b', OP_MATH_R_R_IMMR },
  { "asr",       0x13007c00, 0x7f80fc00, 3, 'b', OP_MATH_R_R_IMMR },
  { "ror",       0x13007c00, 0x7f80fc00, 3, 'b', OP_MATH_R_R_IMMR },

  // These are only in one PDF, missing from another.
  { "autda",     0xdac11800, 0xfffffc00, 2, 'x', OP_MATH_R_R_R },
  { "autdza",    0xdac13be0, 0xffffffe0, 1, 'x', OP_MATH_R_R_R },
  { "autdb",     0xdac11c00, 0xfffffc00, 2, 'x', OP_MATH_R_R_R },
  { "autdzb",    0xdac12fe0, 0xffffffe0, 1, 'x', OP_MATH_R_R_R },
  { "autia",     0xdac11000, 0xfffffc00, 2, 'x', OP_MATH_R_R_R },
  { "autiza",    0xdac133e0, 0xffffffe0, 1, 'x', OP_MATH_R_R_R },
  { "autia1716", 0xd503219f, 0xffffffff, 0, 'x', OP_NONE },
  { "autiasp",   0xd50323bf, 0xffffffff, 0, 'x', OP_NONE },
  { "autiaz",    0xd503239f, 0xffffffff, 0, 'x', OP_NONE },
  { "autib",     0xdac11400, 0xfffffc00, 2, 'x', OP_MATH_R_R_R },
  { "autizb",    0xdac137e0, 0xffffffe0, 1, 'x', OP_MATH_R_R_R },
  { "autib1716", 0xd50321df, 0xffffffff, 0, 'x', OP_NONE },
  { "autibsp",   0xd50323ff, 0xffffffff, 0, 'x', OP_NONE },
  { "autibz",    0xd50323df, 0xffffffff, 0, 'x', OP_NONE },
  { "axflag",    0xd500405f, 0xffffffff, 0, 'x', OP_NONE },

  // C3.2.2 Conditional branch (immediate).
  { "b.eq",      0x54000000, 0xff00001f, 0, 'x', OP_RELATIVE19 },
  { "b.ne",      0x54000001, 0xff00001f, 0, 'x', OP_RELATIVE19 },
  { "b.cs",      0x54000002, 0xff00001f, 0, 'x', OP_RELATIVE19 },
  { "b.cc",      0x54000003, 0xff00001f, 0, 'x', OP_RELATIVE19 },
  { "b.mi",      0x54000004, 0xff00001f, 0, 'x', OP_RELATIVE19 },
  { "b.pl",      0x54000005, 0xff00001f, 0, 'x', OP_RELATIVE19 },
  { "b.vs",      0x54000006, 0xff00001f, 0, 'x', OP_RELATIVE19 },
  { "b.vc",      0x54000007, 0xff00001f, 0, 'x', OP_RELATIVE19 },
  { "b.hi",      0x54000008, 0xff00001f, 0, 'x', OP_RELATIVE19 },
  { "b.ls",      0x54000009, 0xff00001f, 0, 'x', OP_RELATIVE19 },
  { "b.ge",      0x5400000a, 0xff00001f, 0, 'x', OP_RELATIVE19 },
  { "b.lt",      0x5400000b, 0xff00001f, 0, 'x', OP_RELATIVE19 },
  { "b.gt",      0x5400000c, 0xff00001f, 0, 'x', OP_RELATIVE19 },
  { "b.le",      0x5400000d, 0xff00001f, 0, 'x', OP_RELATIVE19 },
  { "b.al",      0x5400000f, 0xff00001f, 0, 'x', OP_RELATIVE19 },

  // C3.2.6 Unconditional branch (immediate).
  { "b",         0x14000000, 0xfc000000, 0, 'x', OP_RELATIVE26 },
  { "bl",        0x94000000, 0xfc000000, 0, 'x', OP_RELATIVE26 },

  // C3.2.7 Unconditional branch (register).
  { "br",        0xd61f0000, 0xfffffc1f, 3, 'b', OP_RET },
  { "blr",       0xd63f0000, 0xfffffc1f, 3, 'b', OP_RET },
  { "ret",       0xd65f0000, 0xfffffc1f, 3, 'b', OP_RET },
  { "eret",      0xd69f0000, 0xfffffc1f, 3, '0', OP_RET },
  { "drps",      0xd6bf0000, 0xfffffc1f, 3, '0', OP_RET },

  // C3.3.5 Load register (literal).
  { "ldr",       0x18000000, 0x3b000000, 2, '0', OP_LD_LITERAL },
  { "ldrsw",     0x18000000, 0x3b000000, 2, 'x', OP_LD_LITERAL },

  // C3.3.8 Load/store register (immediate post-indexed).
  // C3.3.9 Load/store register (immediate pre-indexed).
  { "strb",      0x38000000, 0x3be00000, 3, 'w', OP_LD_ST_IMM_P },
  { "strh",      0x78000000, 0x3be00000, 3, 'w', OP_LD_ST_IMM_P },
  { "str",       0xb8000000, 0x3be00000, 3, '0', OP_LD_ST_IMM_P },
  { "strh",      0x78000000, 0x3be00000, 3, 'w', OP_LD_ST_IMM_P },
  { "ldrb",      0x38400000, 0x3be00000, 3, 'w', OP_LD_ST_IMM_P },
  { "ldrsb",     0x38400000, 0x3be00000, 3, '0', OP_LD_ST_IMM_P },
  { "ldrh",      0x78400000, 0x3be00000, 3, 'w', OP_LD_ST_IMM_P },
  { "ldrsh",     0x78400000, 0x3be00000, 3, '0', OP_LD_ST_IMM_P },
  { "ldr",       0xb8400000, 0x3be00000, 3, '0', OP_LD_ST_IMM_P },

  // C3.3.13 Load/store register (unsigned immediate).
  { "strb",      0x39000000, 0x3be00000, 3, 'w', OP_LD_ST_IMM },
  { "strh",      0x79000000, 0x3be00000, 3, 'w', OP_LD_ST_IMM },
  { "str",       0xb9000000, 0x3be00000, 3, '0', OP_LD_ST_IMM },
  { "strh",      0x79000000, 0x3be00000, 3, 'w', OP_LD_ST_IMM },
  { "ldrb",      0x39400000, 0x3be00000, 3, 'w', OP_LD_ST_IMM },
  { "ldrsb",     0x39400000, 0x3be00000, 3, '0', OP_LD_ST_IMM },
  { "ldrh",      0x79400000, 0x3be00000, 3, 'w', OP_LD_ST_IMM },
  { "ldrsh",     0x79400000, 0x3be00000, 3, '0', OP_LD_ST_IMM },
  { "ldr",       0xb9400000, 0x3be00000, 3, '0', OP_LD_ST_IMM },

  // C3.4.6 PC-rel addressing.
  { "adr",       0x10000000, 0x9f000000, 2, 'x', OP_REG_RELATIVE },
  { "adrp",      0x90000000, 0x9f000000, 2, 'x', OP_REG_PAGE_RELATIVE },

  // C3.6.15 AdvSIMD three different.
  { "saddl",     0x0e200000, 0xbf20fc00, 3, 'v', OP_VECTOR_V_V_V },
  { "saddw",     0x0e201000, 0xbf20fc00, 3, 'v', OP_VECTOR_V_V_V },
  { "ssubl",     0x0e202000, 0xbf20fc00, 3, 'v', OP_VECTOR_V_V_V },
  { "ssubw",     0x0e203000, 0xbf20fc00, 3, 'v', OP_VECTOR_V_V_V },
  { "addhn",     0x0e204000, 0xbf20fc00, 3, 'v', OP_VECTOR_V_V_V },
  { "sabal",     0x0e205000, 0xbf20fc00, 3, 'v', OP_VECTOR_V_V_V },
  { "subhn",     0x0e206000, 0xbf20fc00, 3, 'v', OP_VECTOR_V_V_V },
  { "sabdl",     0x0e207000, 0xbf20fc00, 3, 'v', OP_VECTOR_V_V_V },
  { "smlal",     0x0e208000, 0xbf20fc00, 3, 'i', OP_VECTOR_V_V_V },
  { "sdqmlal",   0x0e209000, 0xbf20fc00, 3, 'i', OP_VECTOR_V_V_V },
  { "smlsl",     0x0e20a000, 0xbf20fc00, 3, 'i', OP_VECTOR_V_V_V },
  { "sqdmlsl",   0x0e20b000, 0xbf20fc00, 3, 'i', OP_VECTOR_V_V_V },
  { "smull",     0x0e20c000, 0xbf20fc00, 3, 'i', OP_VECTOR_V_V_V },
  { "sqdmull",   0x0e20d000, 0xbf20fc00, 3, 'i', OP_VECTOR_V_V_V },
  { "pmull",     0x0e20e000, 0xbf20fc00, 3, 'v', OP_VECTOR_V_V_V },
  { "uaddl",     0x2e200000, 0xbf20fc00, 3, 'v', OP_VECTOR_V_V_V },
  { "uaddw",     0x2e201000, 0xbf20fc00, 3, 'v', OP_VECTOR_V_V_V },
  { "usubl",     0x2e202000, 0xbf20fc00, 3, 'v', OP_VECTOR_V_V_V },
  { "usubw",     0x2e203000, 0xbf20fc00, 3, 'v', OP_VECTOR_V_V_V },
  { "raddhn",    0x2e204000, 0xbf20fc00, 3, 'v', OP_VECTOR_V_V_V },
  { "uabal",     0x2e205000, 0xbf20fc00, 3, 'v', OP_VECTOR_V_V_V },
  { "rsubhn",    0x2e206000, 0xbf20fc00, 3, 'v', OP_VECTOR_V_V_V },
  { "uabdl",     0x2e207000, 0xbf20fc00, 3, 'v', OP_VECTOR_V_V_V },
  { "umlal",     0x2e208000, 0xbf20fc00, 3, 'v', OP_VECTOR_V_V_V },
  { "umlsl",     0x2e20a000, 0xbf20fc00, 3, 'v', OP_VECTOR_V_V_V },
  { "umull",     0x2e20c000, 0xbf20fc00, 3, 'v', OP_VECTOR_V_V_V },

  { "saddl2",    0x4e200000, 0xbf20fc00, 3, 'v', OP_VECTOR_V_V_V },
  { "saddw2",    0x4e201000, 0xbf20fc00, 3, 'v', OP_VECTOR_V_V_V },
  { "ssubl2",    0x4e202000, 0xbf20fc00, 3, 'v', OP_VECTOR_V_V_V },
  { "ssubw2",    0x4e203000, 0xbf20fc00, 3, 'v', OP_VECTOR_V_V_V },
  { "addhn2",    0x4e204000, 0xbf20fc00, 3, 'v', OP_VECTOR_V_V_V },
  { "sabal2",    0x4e205000, 0xbf20fc00, 3, 'v', OP_VECTOR_V_V_V },
  { "subhn2",    0x4e206000, 0xbf20fc00, 3, 'v', OP_VECTOR_V_V_V },
  { "sabdl2",    0x4e207000, 0xbf20fc00, 3, 'v', OP_VECTOR_V_V_V },
  { "smlal2",    0x4e208000, 0xbf20fc00, 3, 'i', OP_VECTOR_V_V_V },
  { "sdqmlal2",  0x4e209000, 0xbf20fc00, 3, 'i', OP_VECTOR_V_V_V },
  { "smlsl2",    0x4e20a000, 0xbf20fc00, 3, 'i', OP_VECTOR_V_V_V },
  { "sqdmlsl2",  0x4e20b000, 0xbf20fc00, 3, 'i', OP_VECTOR_V_V_V },
  { "smull2",    0x4e20c000, 0xbf20fc00, 3, 'i', OP_VECTOR_V_V_V },
  { "sqdmull2",  0x4e20d000, 0xbf20fc00, 3, 'i', OP_VECTOR_V_V_V },
  { "pmull2",    0x4e20e000, 0xbf20fc00, 3, 'v', OP_VECTOR_V_V_V },
  { "uaddl2",    0x6e200000, 0xbf20fc00, 3, 'v', OP_VECTOR_V_V_V },
  { "uaddw2",    0x6e201000, 0xbf20fc00, 3, 'v', OP_VECTOR_V_V_V },
  { "usubl2",    0x6e202000, 0xbf20fc00, 3, 'v', OP_VECTOR_V_V_V },
  { "usubw2",    0x6e203000, 0xbf20fc00, 3, 'v', OP_VECTOR_V_V_V },
  { "raddhn2",   0x6e204000, 0xbf20fc00, 3, 'v', OP_VECTOR_V_V_V },
  { "uabal2",    0x6e205000, 0xbf20fc00, 3, 'v', OP_VECTOR_V_V_V },
  { "rsubhn2",   0x6e206000, 0xbf20fc00, 3, 'v', OP_VECTOR_V_V_V },
  { "uabdl2",    0x6e207000, 0xbf20fc00, 3, 'v', OP_VECTOR_V_V_V },
  { "umlal2",    0x6e208000, 0xbf20fc00, 3, 'v', OP_VECTOR_V_V_V },
  { "umlsl2",    0x6e20a000, 0xbf20fc00, 3, 'v', OP_VECTOR_V_V_V },
  { "umull2",    0x6e20c000, 0xbf20fc00, 3, 'v', OP_VECTOR_V_V_V },

  // C3.4.1 Add/substract (immediate).
  { "add",       0x11000000, 0x7f000000, 3, 'b', OP_MATH_R_R_IMM_SHIFT },
  { "adds",      0x31000000, 0x7f000000, 3, 'b', OP_MATH_R_R_IMM_SHIFT },
  { "sub",       0x51000000, 0x7f000000, 3, 'b', OP_MATH_R_R_IMM_SHIFT },
  { "subs",      0x71000000, 0x7f000000, 3, 'b', OP_MATH_R_R_IMM_SHIFT },

  // C3.4.4 Logical (immediate).
  { "and",       0x12000000, 0x7f800000, 3, 'b', OP_MATH_R_R_IMMR_S },
  { "orr",       0x32000000, 0x7f800000, 3, 'b', OP_MATH_R_R_IMMR_S },
  { "eor",       0x52000000, 0x7f800000, 3, 'b', OP_MATH_R_R_IMMR_S },
  { "ands",      0x72000000, 0x7f800000, 3, 'b', OP_MATH_R_R_IMMR_S },

  // C3.5.1 Add/subtract (extend register).
  { "add",       0x0b200000, 0x7fe00000, 3, 'b', OP_MATH_R_R_R_OPTION },
  { "adds",      0x2b200000, 0x7fe00000, 3, 'b', OP_MATH_R_R_R_OPTION },
  { "sub",       0x4b200000, 0x7fe00000, 3, 'b', OP_MATH_R_R_R_OPTION },
  { "subs",      0x6b200000, 0x7fe00000, 3, 'b', OP_MATH_R_R_R_OPTION },

  // C3.5.2 Add/subtract (shifted).
  { "add",       0x0b000000, 0x7f200000, 3, 'b', OP_MATH_R_R_R_SHIFT },
  { "adds",      0x2b000000, 0x7f200000, 3, 'b', OP_MATH_R_R_R_SHIFT },
  { "sub",       0x4b000000, 0x7f200000, 3, 'b', OP_MATH_R_R_R_SHIFT },
  { "subs",      0x6b000000, 0x7f200000, 3, 'b', OP_MATH_R_R_R_SHIFT },

  // C3.5.3 Add/subtract (width carry).
  { "adc",       0x1a000000, 0x7fe0fc00, 3, 'b', OP_MATH_R_R_R },
  { "adcs",      0x3a000000, 0x7fe0fc00, 3, 'b', OP_MATH_R_R_R },
  { "sbc",       0x5a000000, 0x7fe0fc00, 3, 'b', OP_MATH_R_R_R },
  { "sbcs",      0x7a000000, 0x7fe0fc00, 3, 'b', OP_MATH_R_R_R },

  // C3.5.10 Logical (shifted register) (and aliases).
  { "mov",       0x2a0003e0, 0x7f20ffe0, 2, 'b', OP_MOVE },

  { "and",       0x0a000000, 0x7f200000, 3, 'b', OP_MATH_R_R_R_SHIFT },
  { "bic",       0x0a200000, 0x7f200000, 3, 'b', OP_MATH_R_R_R_SHIFT },
  { "orr",       0x2a000000, 0x7f200000, 3, 'b', OP_MATH_R_R_R_SHIFT },
  { "orn",       0x2a200000, 0x7f200000, 3, 'b', OP_MATH_R_R_R_SHIFT },
  { "eor",       0x4a000000, 0x7f200000, 3, 'b', OP_MATH_R_R_R_SHIFT },
  { "eon",       0x4a200000, 0x7f200000, 3, 'b', OP_MATH_R_R_R_SHIFT },
  { "ands",      0x6a000000, 0x7f200000, 3, 'b', OP_MATH_R_R_R_SHIFT },
  { "bics",      0x6a200000, 0x7f200000, 3, 'b', OP_MATH_R_R_R_SHIFT },

  // C3.6.8 AdvSIMD scalar pairwise.
  { "addp",      0x5ef1b800, 0xfffffc00, 2, 'd', OP_VECTOR_D_V },
  { "addp",      0x5e31b800, 0xbf3ffc00, 2, 'v', OP_VECTOR_V_V_V },

  // C3.6.19 Crypto AES.
  { "aesd",      0x4e285800, 0xfffffc00, 2, 'v', OP_REG_REG_CRYPT },
  { "aese",      0x4e284800, 0xfffffc00, 2, 'v', OP_REG_REG_CRYPT },
  { "aesimc",    0x4e287800, 0xfffffc00, 2, 'v', OP_REG_REG_CRYPT },
  { "aesmc",     0x4e286800, 0xfffffc00, 2, 'v', OP_REG_REG_CRYPT },

  { NULL,        0x00000000, 0x00000000, 0, ' ', 0 }
};

struct _table_arm64_at_op table_arm64_at_op[] =
{
  { "s1e1r",  0x00 },
  { "s1e1w",  0x01 },
  { "s1e0r",  0x02 },
  { "s1e0w",  0x03 },
  { "s1e1rp", 0x08 },
  { "s1e1wp", 0x09 },
  { "s1e2r",  0x40 },
  { "s1e2w",  0x41 },
  { "s12e1r", 0x44 },
  { "s12e1w", 0x45 },
  { "s12e0r", 0x46 },
  { "s12e0w", 0x47 },
  { "s1e3r",  0x60 },
  { "s1e3w",  0x61 },
  { NULL,     0x00 },
};

// C3.2.1 Compare & branch.
struct _table_arm64_compare_branch table_arm64_compare_branch[] =
{
  { "cbz",  0 },
  { "cbnz", 1 },
  { NULL,   0 }
};

// C3.2.3 Exception generation.
struct _table_arm64_exception table_arm64_exception[] =
{
  { "svc",   0, 0, 1 },
  { "hvc",   0, 0, 2 },
  { "smc",   0, 0, 3 },
  { "brk",   1, 0, 0 },
  { "htl",   2, 0, 0 },
  { "dcps1", 5, 0, 1 },
  { "dcps2", 5, 0, 2 },
  { "dcps3", 5, 0, 3 },
  { NULL,    0, 0, 0 }
};

// C3.2.4 System.
struct _table_arm64_system table_arm64_system[] =
{
  { "msr",   0, 0, ARM64_IMM,         4, ARM64_IMM, ARM64_REG_31 },
  { "hint",  0, 0,         3,         2, ARM64_IMM, ARM64_REG_31 },
  { "clrex", 0, 0,         3,         3,         2, ARM64_REG_31 },
  { "dsb",   0, 0,         3,         3,         4, ARM64_REG_31 },
  { "dmb",   0, 0,         3,         3,         5, ARM64_REG_31 },
  { "isb",   0, 0,         3,         3,         6, ARM64_REG_31 },
  { "sys",   0, 1, ARM64_IMM, ARM64_IMM, ARM64_IMM, ARM64_IMM    },
  { "msr",   0, 2, ARM64_IMM, ARM64_IMM, ARM64_IMM, ARM64_IMM    },
  { "sysl",  1, 1, ARM64_IMM, ARM64_IMM, ARM64_IMM, ARM64_IMM    },
  { "mrs",   1, 2, ARM64_IMM, ARM64_IMM, ARM64_IMM, ARM64_IMM    },
  { NULL,    0, 0,         0,         0,         0,         0    }
};

// C3.2.5 Test & branch (immediate).
struct _table_arm64_test_branch table_arm64_test_branch[] =
{
  { "tbz",  0 },
  { "tbnz", 1 },
  { NULL,   0 }
};

// C3.2.7 Unconditional branch (register).
#if 0
struct _table_arm64_uncond_branch table_arm64_uncond_branch[] =
{
  { "br",   0, 31, 0, ARM64_REG_ANY,       0 },
  { "blr",  1, 31, 0, ARM64_REG_ANY,       0 },
  { "ret",  2, 31, 0, ARM64_REG_ANY_OR_30, 0 },
  { "eret", 4, 31, 0, ARM64_REG_31,        0 },
  { "drps", 5, 31, 0, ARM64_REG_31,        0 },
  { NULL,   0,  0, 0,            0,        0 }
};
#endif

// C3.3.1 AdvSIMD load/store multiple structures.
// C3.3.2 AdvSIMD load/store multiple structures (post-indexed).
// C3.3.3 AdvSIMD load/store single structures.
// C3.3.4 AdvSIMD load/store single structures (post-indexed).
// C3.3.6 Load/store exclusive.
// C3.3.7 Load/store no-allocate pair (offset).
// C3.3.10 Load/store register (register offset).
// C3.3.11 Load/store register (unprivileged).
// C3.3.12 Load/store register (unscaled immediate).
// C3.3.14 Load/store register pair (offset).
// C3.3.15 Load/store register pair (post-indexed).
// C3.3.16 Load/store register pair (pre-indexed).
// C3.4.2 Bitfield.
// C3.4.3 Extract.
// C3.4.5 Move wide (immediate).
// C3.5.4 Conditional compare (immediate).
// C3.5.5 Conditional compare (register).
// C3.5.6 Conditional select.
// C3.5.7 Data-processing (1 source).
// C3.5.8 Data-processing (2 source).
// C3.5.9 Data-processing (3 source).
// C3.6.1 AdvSIMD EXT.
// C3.6.2 AdvSIMD TBL/TBX.
// C3.6.3 AdvSIMD ZIP/UZP/TRN.
// C3.6.4 AdvSIMD across lanes.

// C3.6.5 AdvSIMD copy.
struct _table_arm64_simd_copy table_arm64_simd_copy[] =
{
  { "dup",  0, 0, 0x0, ARM64_REG_V_DOT,     ARM64_REG_V_ELEMENT },
  { "dup",  0, 1, 0x0, ARM64_REG_V_SCALAR,  ARM64_REG_V_ELEMENT },
  { "dup",  0, 0, 0x1, ARM64_REG_V_DOT,     ARM64_REG_B         },
  { "smov", 0, 0, 0x5, ARM64_REG_B,         ARM64_REG_V_ELEMENT },
  { "umov", 0, 0, 0x7, ARM64_REG_B,         ARM64_REG_V_ELEMENT },
  { "ins",  1, 0, 0x3, ARM64_REG_V_ELEMENT, ARM64_REG_V_ELEMENT },
  { "ins",  1, 2, 0x0, ARM64_REG_V_ELEMENT, ARM64_REG_B         },
  {  NULL,  0, 0, 0x0, 0,                   0                   },
};

// C3.6.6 AdvSIMD modified immediate.
// C3.6.7 AdvSIMD scalar copy.
// C3.6.9 AdvSIMD scalar shift by immediate.
// C3.6.10 AdvSIMD scalar three different.
// C3.6.11 AdvSIMD scalar three same.
// C3.6.12 AdvSIMD scalar two-reg misc.
// C3.6.13 AdvSIMD scalar x indexed element.
// C3.6.14 AdvSIMD scalar shift by immediate.
// C3.6.16 AdvSIMD three same.
// C3.6.17 AdvSIMD two-reg misc.
// C3.6.18 AdvSIMD vector x indexed element.
// C3.6.20 Crypto three-reg SHA.
// C3.6.21 Crypto two-reg SHA.
// C3.6.22 Floating-point compare.
// C3.6.23 Floating-point conditional compare.
// C3.6.24 Floating-point conditional select.
// C3.6.25 Floating-point data-processing (1 source).
// C3.6.26 Floating-point data-processing (2 source).
// C3.6.27 Floating-point data-processing (3 source).
// C3.6.28 Floating-point immediate.
// C3.6.29 Floating-point fixed-point conversions.
// C3.6.30 Floating-point integer conversions.

