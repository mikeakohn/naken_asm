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

#ifndef NAKEN_ASM_TABLE_ARM64_H
#define NAKEN_ASM_TABLE_ARM64_H

#include <stdint.h>

enum
{
  OP_NONE,
  OP_MATH_R_R_R,
  OP_SCALAR_R_R,
  OP_SCALAR_D_D,
  OP_VECTOR_V_V,
  OP_MATH_R_R_R_OPTION,
};

struct _table_arm64
{
  const char *instr;
  uint32_t opcode;
  uint32_t mask;
  uint8_t type;
};

#define ATTR_SIZE_NONE 0
#define ATTR_SIZE_32 1
#define ATTR_SIZE_64 2
#define ATTR_SIZE_BOTH 3

struct _operand_type_arm64
{
  uint16_t operand_count : 2;
  uint16_t register_map : 4;
  uint16_t size_match : 1;
  uint16_t sizes : 2;
};

extern struct _table_arm64 table_arm64[];
extern struct _operand_type_arm64 operand_type_arm64[];

#endif

