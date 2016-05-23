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
  { "add", { EE_VU_OP_FS, EE_VU_OP_FT, EE_VU_OP_NONE }, 2, 0x000001fd, 0x060007ff, 1 },
  { NULL, { 0, 0, 0 }, 0, 0, 0, 0 }
};

