/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2021 by Michael Kohn
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

