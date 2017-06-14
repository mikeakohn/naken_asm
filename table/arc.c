/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2017 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "table/arc.h"

struct _table_arc table_arc[] =
{
  { "abs",  0x202f0f89, 0xf8ff0fff, OP_B_LIMM },
  { "abs",  0x202f0009, 0xf8ff003f, OP_B_C },
  { "abs",  0x206f0009, 0xf8ff003f, OP_B_U6 },
};

struct _table_arc16 table_arc16[] =
{
  { "abs_s",  0x7811, 0xf81f, OP_B_C },
};

