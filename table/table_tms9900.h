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

#ifndef _TABLE_TMS9900_H
#define _TABLE_TMS9900_H

#include "assembler.h"

enum
{
  OP_NONE,
};

struct _table_tms9900
{
  char *instr;
  unsigned short int opcode;
  unsigned short int mask;
  unsigned char type;
  unsigned char cycles_min;
  unsigned char cycles_max;
};

extern struct _table_tms9900 table_tms9900[];

#endif


