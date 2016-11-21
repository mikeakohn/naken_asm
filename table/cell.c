/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2016 by Michael Kohn
 *
 */

#include "table/cell.h"

struct _table_cell table_cell[] =
{
  { "lqd",    0x34000000, 0xff000000, OP_RT_SYMBOL_RA, FLAG_NONE, 0, 0 },
  { NULL, 0, 0, 0, 0 }
};

