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

#include "table/dotnet.h"

struct _table_dotnet table_dotnet[] =
{
  { "add",  0x58, DOTNET_OP_NONE },
  { NULL,   0x00, DOTNET_OP_ILLEGAL },
};

