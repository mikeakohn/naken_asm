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

#ifndef _TABLE_THUMB_H
#define _TABLE_THUMB_H

#include "assembler.h"

enum
{
  OP_SHIFT,
};

struct _table_thumb
{
  char *instr;
  unsigned short int opcode;
  unsigned short int mask;
  unsigned char type;
  unsigned char cycles;
};

extern struct _table_thumb table_thumb[];

#endif


