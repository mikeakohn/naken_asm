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
  { "nop",     0xd503201f, 0xffffffff, OP_NONE },
  { "abs",     0x5e20b800, 0xff3ffc00, OP_SCALAR_D_D },
  { "abs",     0x0e20b800, 0xbf3ffc00, OP_VECTOR_V_V },
  { "adc",     0x1a000000, 0x7fe0fc00, OP_MATH_R_R_R },
  { "adcs",    0x3a000000, 0x7fe0fc00, OP_MATH_R_R_R },
  { "addg",    0x91800000, 0xffc0c000, OP_MATH_R_R_IMM6_IMM4 },
  { "add",     0x0b200000, 0x7fe00000, OP_MATH_R_R_R_OPTION },
  { "add",     0x11000000, 0x7f000000, OP_MATH_R_R_IMM_SHIFT },
  { "add",     0x0b000000, 0x7f200000, OP_MATH_R_R_R_SHIFT },
  { "add",     0x5e208400, 0xff20fc00, OP_SCALAR_D_D_D },
  { "add",     0x0e208400, 0xbf20fc00, OP_VECTOR_V_V_V },
  { "addhn",   0x0e204000, 0xbf20fc00, OP_VECTOR_V_V_V },
  { "addhn2",  0x0e204000, 0xbf20fc00, OP_VECTOR_V_V_V },
  { "addp",    0x5ef1b800, 0xfffffc00, OP_VECTOR_D_2D },
  { "addp",    0x5e31b800, 0xbf3ffc00, OP_VECTOR_V_V_V },
  { "adds",    0x0b400000, 0x7fe00000, OP_MATH_R_R_R_OPTION },
  { "adds",    0x31000000, 0x7f000000, OP_MATH_R_R_IMM_SHIFT },
  { "adds",    0x2b000000, 0x7f200000, OP_MATH_R_R_R_SHIFT },
  { "addv",    0x0e31b800, 0xbf3ffc00, OP_VECTOR_V_V_TO_SCALAR },
  { "adr",     0x10000000, 0x9f000000, OP_REG_RELATIVE },
  { "adrp",    0x90000000, 0x9f000000, OP_REG_PAGE_RELATIVE },
  { NULL,      0x00000000, 0x00000000, 0 }
};

struct _operand_type_arm64 operand_type_arm64[] =
{
  { 0, 0xe, 0, 1, ATTR_SIZE_NONE }, // OP_NONE
  { 3, 0xe, 1, 1, ATTR_SIZE_BOTH }, // OP_MATH_R_R_R
  //{ 2, 0xc, 1, 1, ATTR_SIZE_BOTH }, // OP_SCALAR_R_R
  { 2, 0xc, 1, 1, ATTR_SIZE_BOTH }, // OP_SCALAR_D_D
  { 2, 0xc, 1, 1, ATTR_SIZE_BOTH }, // OP_VECTOR_V_V
  { 3, 0xe, 1, 1, ATTR_SIZE_BOTH }, // OP_SCALAR_D_D_D
  { 3, 0xe, 1, 1, ATTR_SIZE_BOTH }, // OP_VECTOR_V_V_V
  { 3, 0xe, 0, 0, ATTR_SIZE_BOTH }, // OP_MATH_R_R_R_OPTION
  { 3, 0xc, 0, 0, ATTR_SIZE_BOTH }, // OP_MATH_R_R_IMM_SHIFT
  { 3, 0xe, 0, 0, ATTR_SIZE_BOTH }, // OP_MATH_R_R_R_SHIFT
  { 4, 0xc, 1, 1, ATTR_SIZE_64 },   // OP_MATH_R_R_IMM6_IMM4
  { 2, 0xc, 0, 1, ATTR_SIZE_BOTH }, // OP_VECTOR_D_2D
  { 2, 0xc, 0, 1, ATTR_SIZE_BOTH }, // OP_VECTOR_V_V_TO_SCALAR
  { 2, 0x8, 0, 1, ATTR_SIZE_64 }, // OP_REG_RELATIVE
  { 2, 0x8, 0, 1, ATTR_SIZE_64 }, // OP_REG_PAGE_RELATIVE
};

