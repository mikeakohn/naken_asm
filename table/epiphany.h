/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2019 by Michael Kohn
 *
 */

#ifndef NAKEN_ASM_TABLE_EPIPHANY_H
#define NAKEN_ASM_TABLE_EPIPHANY_H

#include "common/assembler.h"

struct _table_epiphany
{
  char *instr;
  uint32_t opcode;
  uint32_t mask;
  uint8_t type;
  uint8_t size;
  uint8_t cycles_min;
  uint8_t cycles_max;
};

#if 0
// Can't decide if I want to burn the register names into the code

struct _epiphany_specials
{
  char *name;
  uint32_t address;
};
#endif

enum
{
  OP_NONE,
  OP_BRANCH_16,
  OP_BRANCH_32,
  OP_DISP_IMM3_16,
  OP_DISP_IMM11_32,
  OP_INDEX_16,
  OP_INDEX_32,
  OP_POST_MOD_16,
  OP_POST_MOD_32,
  OP_POST_MOD_DISP_32,
  OP_REG_IMM_16,
  OP_REG_IMM_32,
  OP_REG_2_IMM_16,
  OP_REG_2_IMM_32,
  OP_REG_2_IMM5_16,
  OP_REG_2_IMM5_32,
  OP_REG_2_ZERO_16,
  OP_REG_2_ZERO_32,
  OP_REG_3_16,
  OP_REG_3_32,
  OP_REG_2_16,
  OP_REG_2_32,
  OP_REG_1_16,
  OP_REG_1_32,
  OP_NONE_16,
  OP_NUM6_16,
  OP_NONE_32,
  OP_SPECIAL_RN_16,
  OP_RD_SPECIAL_16,
  OP_SPECIAL_RN_32,
  OP_RD_SPECIAL_32,
};

extern struct _table_epiphany table_epiphany[];
//extern struct _epiphany_specials epiphany_specials[];

#endif


