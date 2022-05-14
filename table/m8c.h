/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2022 by Michael Kohn
 *
 */

#ifndef NAKEN_ASM_TABLE_M8C_H
#define NAKEN_ASM_TABLE_M8C_H

#include "common/assembler.h"

enum
{
  OP_NONE,
  OP_A,
  OP_X,
  OP_F,
  OP_SP,
  OP_EXPR,
  OP_INDEX_EXPR,
  OP_INDEX_X_EXPR,
  //OP_INDEX_EXPR_INC,
  OP_REG_INDEX_EXPR,
  OP_REG_INDEX_X_EXPR,
  OP_EXPR_S12,
  OP_EXPR_S12_JUMP,
  OP_EXPR_U16,
};

struct _table_m8c
{
  char *instr;
  uint8_t opcode;
  uint8_t mask;
  uint8_t operand_0;
  uint8_t operand_1;
  uint8_t byte_count;
  uint8_t cycles;
};

extern struct _table_m8c table_m8c[];

#endif

