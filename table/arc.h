/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2017 by Michael Kohn
 *
 */

#ifndef _TABLE_ARC_H
#define _TABLE_ARC_H

#include "common/assembler.h"

enum
{
  OP_NONE,
  OP_B_C,
  OP_B_U6,
  OP_B_LIMM,
  OP_0_C,
  OP_0_U6,
  OP_0_LIMM,
  OP_A_B_C,
  OP_A_B_U6,
  OP_B_B_S12,
  OP_B_B_C,
  OP_B_B_U6,
  OP_A_LIMM_C,
  OP_A_B_LIMM,
  OP_B_B_LIMM,
  OP_0_B_C,
  OP_0_B_U6,
  OP_0_B_LIMM,
  OP_0_LIMM_C,
  OP_B_C_O9,    // offset 9 rather than S9
  OP_B_U6_O9,
  // 16 bit
  OP_C_B_U3,
  OP_B_B_H,
  OP_B_B_U,
  OP_B_SP_U7,
  OP_SP_SP_U7,
  OP_R0_GP_S9,
  OP_B_B_U7, 
  OP_B_B_U5, 
};

#define F_F  1
#define F_CC 2
#define F_D 3

struct _table_arc
{
  char *instr;
  uint32_t opcode;
  uint32_t mask;
  uint8_t type;
  uint8_t flags;
};

struct _table_arc16
{
  char *instr;
  uint16_t opcode;
  uint16_t mask;
  uint8_t type;
};

extern struct _table_arc table_arc[];
extern struct _table_arc16 table_arc16[];

#endif


