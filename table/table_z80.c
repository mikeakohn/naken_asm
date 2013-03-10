#include <stdio.h>
#include <stdlib.h>
#include "table_z80.h"

// http://search.cpan.org/~pscust/Asm-Z80-Table-0.02/lib/Asm/Z80/Table.pm

struct _table_z80 table_z80[] =
{
  //{ "mov", { OP_REG, OP_A, OP_NONE }, 7 }, // 0xFF
  { NULL, { OP_NONE, OP_NONE, OP_NONE }, 7 }, // 0xFF
};

