/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2017 by Michael Kohn
 *
 */

#ifndef _TABLE_SUPER_FX_H
#define _TABLE_SUPER_FX_H

#include "common/assembler.h"

enum
{
  OP_NONE,
  OP_REG,
  OP_NUM,
  OP_OFFSET,
  OP_REG_NUM,
  OP_REG_WORD,
  OP_REG_MEM,
  OP_MEM_REG,
  OP_REG_SMEM,
  OP_SMEM_REG,
};

struct _table_super_fx
{
  char *instr;
  uint8_t opcode;
  uint8_t alt;
  uint8_t mask;
  uint8_t type;
  uint16_t reg_mask;
};

extern struct _table_super_fx table_super_fx[];

#endif


