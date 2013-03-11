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
  OP_A_REG8,
  OP_REG8,
  OP_A_REG_IHALF,
  OP_A_INDEX,
  OP_REG8_CB,
};

struct _table_z80
{
  char *instr;
  unsigned short int opcode;
  unsigned short int mask;
  unsigned char type;
};

extern struct _table_z80 table_z80[];

#endif


