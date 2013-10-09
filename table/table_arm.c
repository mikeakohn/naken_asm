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

#include "table_arm.h"

struct _table_arm table_arm[] =
{
  { "lsl", 0x0000, 0xf800, OP_SHIFT, 2 },
  { NULL, 0, 0, 0, 0 }
};



