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

#ifndef NAKEN_ASM_TABLE_ARM64_H
#define NAKEN_ASM_TABLE_ARM64_H

#include <stdint.h>

enum
{
  OP_NONE,
  OP_MATH_R_R_R_OPTION,
  OP_MATH_R_R_R_SHIFT,
  OP_MATH_R_R_IMM_SHIFT,
  OP_MATH_R_R_IMM6_IMM4,
  OP_MOVE,
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
  OP_RET,
  OP_LD_ST_IMM_P,
  OP_LD_ST_IMM,
  OP_LD_LITERAL,
  // Alias for subs / adds.
  OP_ALIAS_REG_IMM,
  // Other SIMD.
  OP_SCALAR_SHIFT_IMM,
  OP_VECTOR_SHIFT_IMM,
  OP_VECTOR_V_V_V_FPU,
  OP_VECTOR_V_V_FPU,
  OP_VECTOR_D_D_D_FPU,
  OP_VECTOR_D_D_FPU,
  //OP_VECTOR_V_V_V_CMP_FPU,
};

enum
{
  ARM64_REG_B,
  ARM64_REG_V_SCALAR,
  ARM64_REG_V_DOT,
  ARM64_REG_V_ELEMENT,
};

#define ARM64_IMM           0xff
#define ARM64_REG_31        0xfe
#define ARM64_REG_ANY       0xfd
#define ARM64_REG_ANY_OR_30 0xfc

// reg_type is:
// 0 = no regster.
// w = 32 bit register (w23).
// x = 64 bit register (x23).
// b = both - either 32 or 64 bit register (w23 or x23).
// v = vector with size (v23.16b).
// i = vector with size (v23.16b) and an index.
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

struct _table_arm64_compare_branch
{
  const char *name;
  uint8_t op;
};

struct _table_arm64_exception
{
  const char *name;
  uint8_t opc;
  uint8_t op2;
  uint8_t ll;
};

struct _table_arm64_system
{
  const char *name;
  uint8_t l;
  uint8_t op0;
  uint8_t op1;
  uint8_t crn;
  uint8_t op2;
  uint8_t rt;
};

struct _table_arm64_test_branch
{
  const char *name;
  uint8_t op;
};

struct _table_arm64_simd_copy
{
  const char *instr;
  uint8_t q;
  uint8_t op;
  uint8_t imm4;
  uint8_t reg_rd;
  uint8_t reg_rn;
};

#if 0
struct _table_arm64_uncond_branch
{
  const char *name;
  uint8_t opc;
  uint8_t op2;
  uint8_t op3;
  uint8_t rn;
  uint8_t op4;
};
#endif

extern struct _table_arm64 table_arm64[];
extern struct _table_arm64_at_op table_arm64_at_op[];
extern struct _table_arm64_compare_branch table_arm64_compare_branch[];
extern struct _table_arm64_exception table_arm64_exception[];
extern struct _table_arm64_system table_arm64_system[];
extern struct _table_arm64_test_branch table_arm64_test_branch[];
//extern struct _table_arm64_uncond_branch table_arm64_uncond_branch[];
extern struct _table_arm64_simd_copy table_arm64_simd_copy[];

#endif

