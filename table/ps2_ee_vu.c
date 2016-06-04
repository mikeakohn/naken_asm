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

struct _table_ps2_ee_vu table_ps2_ee_vu_upper[] =
{
  { "add", { EE_VU_OP_FD, EE_VU_OP_FS, EE_VU_OP_FT }, 3, 0x00000028, 0x0600003f, FLAG_DEST },
  { "addx", { EE_VU_OP_FD, EE_VU_OP_FS, EE_VU_OP_FT }, 3, 0x00000000, 0x0600003f, FLAG_DEST | FLAG_BC },
  { "addy", { EE_VU_OP_FD, EE_VU_OP_FS, EE_VU_OP_FT }, 3, 0x00000001, 0x0600003f, FLAG_DEST | FLAG_BC },
  { "addz", { EE_VU_OP_FD, EE_VU_OP_FS, EE_VU_OP_FT }, 3, 0x00000002, 0x0600003f, FLAG_DEST | FLAG_BC },
  { "addw", { EE_VU_OP_FD, EE_VU_OP_FS, EE_VU_OP_FT }, 3, 0x00000003, 0x0600003f, FLAG_DEST | FLAG_BC },
  { "nop", { EE_VU_OP_NONE, EE_VU_OP_NONE, EE_VU_OP_NONE }, 0, 0x000002ff, 0x07ffffff, FLAG_NONE },
  { NULL, { 0, 0, 0 }, 0, 0, 0, 0 }
};

struct _table_ps2_ee_vu table_ps2_ee_vu_lower[] =
{
  { "nop", { EE_VU_OP_NONE, EE_VU_OP_NONE, EE_VU_OP_NONE }, 0, 0x8000033c, 0xffffffff, FLAG_NONE },
  { NULL, { 0, 0, 0 }, 0, 0, 0, 0 }
};

