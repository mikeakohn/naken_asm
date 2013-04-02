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

#ifndef _TABLE_AVR8_H
#define _TABLE_AVR8_H

#include "assembler.h"

enum
{
  OP_NONE,
  OP_BRANCH_S_K,
  OP_BRANCH_K,
  OP_TWO_REG,
  OP_REG_IMM,
  OP_ONE_REG,
  OP_REG_BIT,
  OP_REG_IMM_WORD,
  OP_IOREG_BIT,
  OP_SREG_BIT,
  OP_REG_4,
  OP_IN,
  OP_OUT,

#if 0
  OP_K,
  OP_RD_RR,
  OP_RD_B,
  OP_S_B,
  OP_P_B,
  OP_RD_K,
#endif
};

struct _table_avr8
{
  char *instr;
  unsigned short int opcode;
  unsigned short int mask;
  unsigned char type;
  unsigned char cycles_min;
  unsigned char cycles_max;
};

extern struct _table_avr8 table_avr8[];

#endif


