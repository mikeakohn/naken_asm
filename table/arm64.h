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

#ifndef NAKEN_ASM_TABLE_ARM64_H
#define NAKEN_ASM_TABLE_ARM64_H

#include <stdint.h>

enum
{
  OP_NONE,
  OP_MATH_R_R_R_OPTION,
  OP_MATH_R_R_IMM_SHIFT,
  OP_MATH_R_R_R_SHIFT,
  OP_MATH_R_R_IMM6_IMM4,
  OP_REG_RELATIVE,
  OP_REG_PAGE_RELATIVE,
  OP_VECTOR_D_V,
  OP_VECTOR_V_V_TO_SCALAR,
  OP_MATH_R_R_IMMR_S,
  OP_MATH_R_R_IMMR,
  OP_AT,
  OP_RELATIVE19,
  OP_RELATIVE26,
  OP_REG_BITFIELD,
  // Similar register instructions.
  OP_SCALAR_D_D,
  OP_SCALAR_D_D_D,
  OP_VECTOR_V_V,
  OP_VECTOR_V_V_V,
  OP_MATH_R_R_R,
  OP_REG_REG_CRYPT,
};

// reg_type is:
// w = 32 bit register (w23).
// x = 64 bit register (x23).
// b = either 32 or 64 bit register (w23 or x23).
// v = vector with size (v23.16b).
// d = scalar vector (double) (d23).
// h = scalar vector (half) (h23).
// s = scalar vector (short) (h23).
// b = scalar vector (byte) (b23).

struct _table_arm64
{
  const char *instr;
  uint32_t opcode;
  uint32_t mask;
  uint8_t operand_count;
  uint8_t reg_type;
  uint8_t type;
};

struct _table_arm64_at_op
{
  const char *name;
  uint8_t value;
};

extern struct _table_arm64 table_arm64[];
extern struct _table_arm64_at_op table_arm64_at_op[];

#endif

