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

#ifndef _TABLE_680x0_H
#define _TABLE_680x0_H

#include "assembler.h"

enum
{
  OP_NONE,
  OP_SINGLE_EA,
  OP_SINGLE_EA_NO_SIZE,
  OP_SINGLE_EA_TO_ADDR,
  OP_IMMEDIATE,
  OP_SHIFT_EA,
  OP_SHIFT,
  OP_REG_AND_EA,
  OP_VECTOR,
  OP_VECTOR3,
  OP_AREG,
  OP_REG,
  OP_EA_AREG,
  OP_EA_DREG,
  OP_LOAD_EA,
  OP_QUICK,
  OP_MOVE_QUICK,
  OP_MOVE_FROM_CCR,
  OP_MOVE_TO_CCR,
  OP_MOVE_FROM_SR,
  OP_MOVEA,
  OP_CMPM,
  OP_BCD,
  OP_EXTENDED,
  OP_ROX_MEM,
  OP_ROX,
  OP_EXCHANGE,
  OP_REG_EA_NO_SIZE,
  OP_EXTRA_IMM_EA,
  OP_EA_DREG_WL,
  OP_LOGIC_CCR,
  OP_DISPLACEMENT,
  OP_EXT,
  OP_LINK,
  OP_DIV_MUL,
  OP_MOVEP,
};

struct _table_680x0
{
  char *instr;
  unsigned short int opcode;
  unsigned short int mask;
  char type;
};

#endif


