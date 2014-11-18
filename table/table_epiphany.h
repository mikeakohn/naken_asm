/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2014 by Michael Kohn
 *
 */

#ifndef _TABLE_EPIPHANY_H
#define _TABLE_EPIPHANY_H

#include "assembler.h"

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
};

extern struct _table_epiphany table_epiphany[];

#endif


