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
  { "adc",       0x1a000000, 0x7fe0fc00, 3, 'b', OP_MATH_R_R_R },
  { "adcs",      0x3a000000, 0x7fe0fc00, 3, 'w', OP_MATH_R_R_R },
  { "addg",      0x91800000, 0xffc0c000, 4, 'x', OP_MATH_R_R_IMM6_IMM4 },
  { "add",       0x0b200000, 0x7fe00000, 3, 'b', OP_MATH_R_R_R_OPTION },
  { "add",       0x11000000, 0x7f000000, 3, 'b', OP_MATH_R_R_IMM_SHIFT },
  { "add",       0x0b000000, 0x7f200000, 3, 'b', OP_MATH_R_R_R_SHIFT },
  { "add",       0x5e208400, 0xff20fc00, 3, 'd', OP_SCALAR_D_D_D },
  { "add",       0x0e208400, 0xbf20fc00, 3, 'v', OP_VECTOR_V_V_V },
  { "addhn",     0x0e204000, 0xbf20fc00, 3, 'v', OP_VECTOR_V_V_V },
  { "addhn2",    0x0e204000, 0xbf20fc00, 3, 'v', OP_VECTOR_V_V_V },
  { "addp",      0x5ef1b800, 0xfffffc00, 2, 'd', OP_VECTOR_D_V },
  { "addp",      0x5e31b800, 0xbf3ffc00, 2, 'v', OP_VECTOR_V_V_V },
  { "adds",      0x0b400000, 0x7fe00000, 3, 'b', OP_MATH_R_R_R_OPTION },
  { "adds",      0x31000000, 0x7f000000, 3, 'b', OP_MATH_R_R_IMM_SHIFT },
  { "adds",      0x2b000000, 0x7f200000, 3, 'b', OP_MATH_R_R_R_SHIFT },
  { "addv",      0x0e31b800, 0xbf3ffc00, 2, 'd', OP_VECTOR_V_V_TO_SCALAR },
  { "adr",       0x10000000, 0x9f000000, 2, 'x', OP_REG_RELATIVE },
  { "adrp",      0x90000000, 0x9f000000, 2, 'x', OP_REG_PAGE_RELATIVE },
  { "aesd",      0x4e285800, 0xfffffc00, 2, 'v', OP_REG_REG_CRYPT },
  { "aese",      0x4e284800, 0xfffffc00, 2, 'v', OP_REG_REG_CRYPT },
  { "aesimc",    0x4e287800, 0xfffffc00, 2, 'v', OP_REG_REG_CRYPT },
  { "aesmc",     0x4e286800, 0xfffffc00, 2, 'v', OP_REG_REG_CRYPT },
  { "and",       0x0e201c00, 0xbfe0fc00, 3, 'v', OP_VECTOR_V_V_V },
  { "and",       0x12000000, 0x7f800000, 3, 'b', OP_MATH_R_R_IMMR_S },
  { "and",       0x0a000000, 0x7f200000, 3, 'b', OP_MATH_R_R_R_SHIFT },
  { "ands",      0x72000000, 0x7f800000, 3, 'v', OP_MATH_R_R_IMMR_S },
  { "ands",      0x6a000000, 0x7f200000, 3, 'b', OP_MATH_R_R_R_SHIFT },
  { "asr",       0x1ac02800, 0x7fe0fc00, 3, 'b', OP_MATH_R_R_R },
  { "asr",       0x13007c00, 0x7f80fc00, 3, 'b', OP_MATH_R_R_IMMR },
  { "asrv",      0x1ac02800, 0x7fe0fc00, 3, 'b', OP_MATH_R_R_R },
  { "at",        0xd5807800, 0xfff8fe00, 2, 'b', OP_AT },
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
  { "b",         0x14000000, 0xfc000000, 0, 'x', OP_RELATIVE26 },
  { "bcax",      0xce200000, 0xffe08000, 4, 'v', OP_REG_REG_CRYPT },
  { "bfc",       0xce200000, 0x7f8003e0, 3, 'b', OP_REG_BITFIELD },

  { "ret",       0xd65f0000, 0xfffffc1f, 3, 'b', OP_RET },
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

// C3.2.2 Conditional branch (immediate).

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

// C3.2.6 Unconditional branch (immediate).
struct _table_arm64_uncond_branch table_arm64_uncond_branch[] =
{
  { "br",   0, 31, 0, ARM64_REG_ANY,       0 },
  { "blr",  1, 31, 0, ARM64_REG_ANY,       0 },
  { "ret",  2, 31, 0, ARM64_REG_ANY_OR_30, 0 },
  { "eret", 4, 31, 0, ARM64_REG_31,        0 },
  { "drps", 5, 31, 0, ARM64_REG_31,        0 },
  { NULL,   0,  0, 0,            0,        0 }
};

// C3.2.7 Unconditional branch (register).
// C3.3.1 AdvSIMD load/store multiple structures.
// C3.3.2 AdvSIMD load/store multiple structures (post-indexed).
// C3.3.3 AdvSIMD load/store single structures.
// C3.3.4 AdvSIMD load/store single structures (post-indexed).
// C3.3.5 Load register (literal).
// C3.3.6 Load/store exclusive.
// C3.3.7 Load/store no-allocate pair (offset).
// C3.3.8 Load/store register (immediate post-indexed).
// C3.3.9 Load/store register (immediate pre-indexed).
// C3.3.10 Load/store register (register offset).
// C3.3.11 Load/store register (unprivileged).
// C3.3.12 Load/store register (unscaled immediate).
// C3.3.13 Load/store register (unsigned immediate).
// C3.3.14 Load/store register pair (offset).
// C3.3.15 Load/store register pair (post-indexed).
// C3.3.16 Load/store register pair (pre-indexed).
// C3.4.1 Add/substrict (immediate).
// C3.4.2 Bitfield.
// C3.4.3 Extract.
// C3.4.4 Logical (immediate).
// C3.4.5 Move wide (immediate).
// C3.4.6 PC-rel addressing.
// C3.5.1 Add/subtract (extend register).
// C3.5.2 Add/subtract (shifted).
// C3.5.3 Add/subtract (width carry).
// C3.5.4 Conditional compare (immediate).
// C3.5.5 Conditional compare (register).
// C3.5.6 Conditional select.
// C3.5.7 Data-processing (1 source).
// C3.5.8 Data-processing (2 source).
// C3.5.9 Data-processing (3 source).
// C3.5.10 Logical (shifted register).
// C3.6.1 AdvSIMD EXT.
// C3.6.2 AdvSIMD TBL/TBX.
// C3.6.3 AdvSIMD ZIP/UZP/TRN.
// C3.6.4 AdvSIMD across lanes.
// C3.6.5 AdvSIMD copy.
// C3.6.6 AdvSIMD modified immediate.
// C3.6.7 AdvSIMD scalar copy.
// C3.6.8 AdvSIMD scalar pairwise.
// C3.6.9 AdvSIMD scalar shift by immediate.
// C3.6.10 AdvSIMD scalar three different.
// C3.6.11 AdvSIMD scalar three same.
// C3.6.12 AdvSIMD scalar two-reg misc.
// C3.6.13 AdvSIMD scalar x indexed element.
// C3.6.14 AdvSIMD scalar shift by immediate.
// C3.6.15 AdvSIMD three different.
// C3.6.16 AdvSIMD three same.
// C3.6.17 AdvSIMD two-reg misc.
// C3.6.18 AdvSIMD vector x indexed element.
// C3.6.19 Crypto AES.
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



