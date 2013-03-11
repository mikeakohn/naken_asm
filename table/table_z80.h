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
};

struct _table_z80_a_reg
{
  char *instr;
  unsigned char opcode;
};

extern struct _table_z80_a_reg table_z80_a_reg[];

#endif


