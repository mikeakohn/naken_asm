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

#include <stdlib.h>

#include "table/ps2_ee_vu.h"

struct _table_ps2_ee_vu table_ps2_ee_vu[] =
{
  { "add", { EE_VU_OP_FS, EE_VU_OP_FT, EE_VU_OP_NONE }, 2, 0x000001fd, 0x060007ff, FLAG_NONE, 1, IS_UPPER },
  { "addx", { EE_VU_OP_FD, EE_VU_OP_FS, EE_VU_OP_FT }, 3, 0x01000000, 0xffe0003c, FLAG_DEST | FLAG_BC, 1, IS_UPPER },
  { "addy", { EE_VU_OP_FD, EE_VU_OP_FS, EE_VU_OP_FT }, 3, 0x00800000, 0xffe0003c, FLAG_DEST | FLAG_BC, 1, IS_UPPER },
  { "addz", { EE_VU_OP_FD, EE_VU_OP_FS, EE_VU_OP_FT }, 3, 0x00400000, 0xffe0003c, FLAG_DEST | FLAG_BC, 1, IS_UPPER },
  { "addw", { EE_VU_OP_FD, EE_VU_OP_FS, EE_VU_OP_FT }, 3, 0x00200000, 0xffe0003c, FLAG_DEST | FLAG_BC, 1, IS_UPPER },
  { NULL, { 0, 0, 0 }, 0, 0, 0, 0, 0 }
};

