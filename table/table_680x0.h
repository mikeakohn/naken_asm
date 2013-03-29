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
  OP_IMMEDIATE,
  OP_SHIFT_EA,
  OP_SHIFT,
  OP_REG_AND_EA,
  OP_VECTOR,
  OP_AREG,
  OP_REG,
  OP_EA_AREG,
  OP_EA_DREG,
};

struct _table_680x0
{
  char *instr;
  unsigned short int opcode;
  unsigned short int mask;
  char type;
};

#endif


