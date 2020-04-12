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

#include <stdlib.h>

#include "table/arm64.h"

struct _table_arm64 table_arm64[] =
{
  { "nop",     0xd503201f, 0xffffffff, OP_NONE },
  { "abs",     0x5e20b800, 0xff3ffc00, OP_SCALAR_D_D },
  { "abs",     0x0e20b800, 0xbf3ffc00, OP_VECTOR_V_V },
  { "adc",     0x1a000000, 0x7fe0fc00, OP_MATH_R32_R32_R32 },
  { "adc",     0x9a000000, 0x7fe0fc00, OP_MATH_R64_R64_R64 },
  { "adcs",    0x3a000000, 0x7fe0fc00, OP_MATH_R32_R32_R32 },
  { "adcs",    0xba000000, 0x7fe0fc00, OP_MATH_R64_R64_R64 },
};

struct _operand_type_arm64 operand_type_arm64[] =
{
  { 0, 0xe, 0, ATTR_SIZE_NONE }, // OP_NONE
  { 3, 0xe, 1, ATTR_SIZE_32 },   // OP_MATH_R32_R32_R32
  { 3, 0xe, 1, ATTR_SIZE_64 },   // OP_MATH_R64_R64_R64
  { 2, 0xc, 1, ATTR_SIZE_BOTH }, // OP_SCALAR_R_R
  { 2, 0xc, 1, ATTR_SIZE_BOTH }, // OP_SCALAR_D_D
  { 2, 0xc, 1, ATTR_SIZE_BOTH }, // OP_VECTOR_V_V
};

