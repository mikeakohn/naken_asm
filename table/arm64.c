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
  { "nop",     0xd503201f, 0xffffffff, 0, ' ', OP_NONE },
  { "abs",     0x5e20b800, 0xff3ffc00, 2, 'd', OP_SCALAR_D_D },
  { "abs",     0x0e20b800, 0xbf3ffc00, 2, 'v', OP_VECTOR_V_V },
  { "adc",     0x1a000000, 0x7fe0fc00, 3, 'b', OP_MATH_R_R_R },
  { "adcs",    0x3a000000, 0x7fe0fc00, 3, 'w', OP_MATH_R_R_R },
  { "addg",    0x91800000, 0xffc0c000, 4, 'x', OP_MATH_R_R_IMM6_IMM4 },
  { "add",     0x0b200000, 0x7fe00000, 3, 'b', OP_MATH_R_R_R_OPTION },
  { "add",     0x11000000, 0x7f000000, 3, 'b', OP_MATH_R_R_IMM_SHIFT },
  { "add",     0x0b000000, 0x7f200000, 3, 'b', OP_MATH_R_R_R_SHIFT },
  { "add",     0x5e208400, 0xff20fc00, 3, 'd', OP_SCALAR_D_D_D },
  { "add",     0x0e208400, 0xbf20fc00, 3, 'v', OP_VECTOR_V_V_V },
  { "addhn",   0x0e204000, 0xbf20fc00, 3, 'v', OP_VECTOR_V_V_V },
  { "addhn2",  0x0e204000, 0xbf20fc00, 3, 'v', OP_VECTOR_V_V_V },
  { "addp",    0x5ef1b800, 0xfffffc00, 2, 'd', OP_VECTOR_D_V },
  { "addp",    0x5e31b800, 0xbf3ffc00, 2, 'v', OP_VECTOR_V_V_V },
  { "adds",    0x0b400000, 0x7fe00000, 3, 'b', OP_MATH_R_R_R_OPTION },
  { "adds",    0x31000000, 0x7f000000, 3, 'b', OP_MATH_R_R_IMM_SHIFT },
  { "adds",    0x2b000000, 0x7f200000, 3, 'b', OP_MATH_R_R_R_SHIFT },
  { "addv",    0x0e31b800, 0xbf3ffc00, 2, 'd', OP_VECTOR_V_V_TO_SCALAR },
  { "adr",     0x10000000, 0x9f000000, 2, 'x', OP_REG_RELATIVE },
  { "adrp",    0x90000000, 0x9f000000, 2, 'x', OP_REG_PAGE_RELATIVE },
  { "aesd",    0x4e285800, 0xfffffc00, 2, 'v', OP_REG_REG_CRYPT },
  { "aese",    0x4e284800, 0xfffffc00, 2, 'v', OP_REG_REG_CRYPT },
  { "aesimc",  0x4e287800, 0xfffffc00, 2, 'v', OP_REG_REG_CRYPT },
  { "aesmc",   0x4e286800, 0xfffffc00, 2, 'v', OP_REG_REG_CRYPT },
  { "and",     0x0e201c00, 0xbfe0fc00, 3, 'v', OP_VECTOR_V_V_V },
  { "and",     0x12000000, 0x7f800000, 3, 'b', OP_MATH_R_R_IMMR_S },
  { "and",     0x0a000000, 0x7f200000, 3, 'b', OP_MATH_R_R_R_SHIFT },
  { "ands",    0x72000000, 0x7f800000, 3, 'v', OP_MATH_R_R_IMMR_S },
  { "ands",    0x6a000000, 0x7f200000, 3, 'b', OP_MATH_R_R_R_SHIFT },
  { "asr",     0x1ac02800, 0x7fe0fc00, 3, 'b', OP_MATH_R_R_R },
  { "asr",     0x13007c00, 0x7f80fc00, 3, 'b', OP_MATH_R_R_IMMR },
  { "asrv",    0x1ac02800, 0x7fe0fc00, 3, 'b', OP_MATH_R_R_R },
  { NULL,      0x00000000, 0x00000000, 0, ' ', 0 }
};

