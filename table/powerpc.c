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

#include "table/powerpc.h"

struct _table_powerpc table_powerpc[] =
{
  { "add",  0xec000000, 0xfc0007fe, OP_R_R_R, 0, 0 },
  { "addo", 0xec000400, 0xfc0007fe, OP_R_R_R, 0, 0 },
  //{ "blcr", 0x4c000000, 0xfc00f801, OP_BRANCH, 0, 0 },
  //{ "blcrl", 0x4c000001, 0xfc00f801, OP_BRANCH, 0, 0 },
  { NULL, 0, 0, 0, 0 }
};

