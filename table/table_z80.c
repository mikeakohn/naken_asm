#include <stdio.h>
#include <stdlib.h>
#include "table_z80.h"

// http://search.cpan.org/~pscust/Asm-Z80-Table-0.02/lib/Asm/Z80/Table.pm

struct _table_z80_a_reg table_z80_a_reg[] =
{
  { "adc", 0x88 },
  { "add", 0x80 },
  { "ld", 0x78 },
  { "sbc", 0x98 },
  { NULL, 0x00 },
};

