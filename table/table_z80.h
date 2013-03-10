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

struct _table_z80
{
  char *name;
  char op[3];
  char range;   // range of register or page (r0, r1, r2 etc)
};

extern struct _table_z80 table_z80[];

#endif


