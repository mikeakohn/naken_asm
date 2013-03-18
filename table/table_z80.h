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
  OP_INDEX_XY,
  OP_JR_COND_ADDRESS,
  OP_REG8_REG8,
  OP_REG8_REG_IHALF,
  OP_REG_IHALF_REG8,
  OP_REG_IHALF_REG_IHALF,
  OP_REG8_NUMBER8,
  OP_REG8_INDEX_HL,
  OP_REG8_INDEX,
  OP_INDEX_HL_REG8,
  OP_INDEX_HL_NUMBER8,
  OP_INDEX_REG8,
  OP_INDEX_NUMBER8,
  OP_A_INDEX_BC,
  OP_A_INDEX_DE,
  OP_A_INDEX_ADDRESS,
  OP_INDEX_BC_A,
  OP_INDEX_DE_A,
  OP_INDEX_ADDRESS_A,
  OP_IR_A,
  OP_A_IR,
  OP_REG16_ADDRESS,
  OP_XY_ADDRESS,
  OP_REG16_INDEX_ADDRESS,
  OP_HL_INDEX_ADDRESS,
  OP_XY_INDEX_ADDRESS,
  OP_INDEX_ADDRESS_REG16,
  OP_INDEX_ADDRESS_HL,
  OP_INDEX_ADDRESS_XY,
  OP_SP_HL,
  OP_SP_XY,
  OP_INDEX_ADDRESS8_A,
  OP_INDEX_C_REG8,
  OP_INDEX_C_ZERO,
  OP_REG16P,
  OP_BIT_INDEX_V2,
  OP_BIT_INDEX_REG8,
  OP_COND,
  OP_REG8_CB,
  OP_INDEX_HL_CB,
  OP_RESTART_ADDRESS,
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

#if 0
struct _table_z80_4_byte
{
  char *instr;
  unsigned char opcode[4];
  unsigned char mask[4];
  unsigned char type;
  unsigned char cycles;
};
#endif

extern struct _table_z80 table_z80[];

#endif


