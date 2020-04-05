/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2020 by Michael Kohn
 *
 */

#include <stdlib.h>

#include "table/arm64.h"

struct _table_arm64 table_arm64[] = {
  { "nop",     0xd503201f, 0xffffffff, OP_NONE },
  { "adc",     0x1a000000, 0x7fe0fc00, OP_MATH_R32_R32_R32 },
  { "adc",     0x9a000000, 0x7fe0fc00, OP_MATH_R64_R64_R64 },
};

