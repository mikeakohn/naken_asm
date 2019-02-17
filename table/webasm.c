/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2019 by Michael Kohn
 *
 */

#include <stdlib.h>

#include "table/webasm.h"

struct _table_webasm table_webasm[] =
{
  { "add",            0x58, WEBASM_OP_NONE },
  { NULL,             0x00, WEBASM_OP_ILLEGAL },
};

