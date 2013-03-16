/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2013 by Michael Kohn
 *
 */

#ifndef _TABLE_Z80_H
#define _TABLE_Z80_H

#include "assembler.h"

enum
{
  OP_NONE,
  OP_NONE16,
  OP_NONE24,
  OP_A_REG8,
  OP_REG8,
  OP_A_REG_IHALF,
  OP_A_INDEX,
  OP_A_NUMBER8,
  OP_REG8_CB,
  OP_HL_REG16_1,
  OP_HL_REG16_2,
  OP_XY_REG16,
  OP_A_INDEX_HL,
  OP_INDEX_HL,
  OP_NUMBER8,
  OP_REG_IHALF,
  OP_INDEX,
  OP_BIT_REG8,
  OP_BIT_INDEX_HL,
  OP_BIT_INDEX,
  OP_ADDRESS,
  OP_COND_ADDRESS,
  OP_REG8_V2,
  OP_REG_IHALF_V2,
  OP_REG16,
  OP_XY,
  OP_INDEX_SP_HL,
  OP_INDEX_SP_XY,
  OP_AF_AF_TICK,
  OP_DE_HL,
  OP_IM_NUM,
  OP_A_INDEX_N,
  OP_REG8_INDEX_C,
  OP_F_INDEX_C,
};

struct _table_z80
{
  char *instr;
  unsigned short int opcode;
  unsigned short int mask;
  unsigned char type;
  unsigned char cycles:5;
  unsigned char cycles_max:3; // add these 3 bits to cycles for max
};

extern struct _table_z80 table_z80[];

#endif


