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
  { "abs",  0x266f7009, 0xffff703f, OP_0_U6 },
  { "abs",  0x262f7f89, 0xffff7fff, OP_0_LIMM },
  { "abs",  0x262f7009, 0xffff703f, OP_0_C },
  { "abs",  0x206f0009, 0xf8ff003f, OP_B_U6 },
  { "abs",  0x202f0f89, 0xf8ff0fff, OP_B_LIMM },
  { "abs",  0x202f0009, 0xf8ff003f, OP_B_C },
};

struct _table_arc16 table_arc16[] =
{
  { "abs_s",  0x7811, 0xf81f, OP_B_C },
};

