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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "common/memory.h"
#include "fileio/write_bin.h"

int write_bin(struct _memory *memory, FILE *out)
{
  uint32_t n;

  for (n = memory->low_address; n <= memory->high_address; n++)
  {
    putc(memory_read_m(memory, n), out);
  }

  return 0;
}

