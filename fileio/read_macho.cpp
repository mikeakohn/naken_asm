/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2024 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>

#include "common/assembler.h"
#include "common/Symbols.h"
#include "fileio/read_macho.h"

int read_macho(
  const char *filename,
  Memory *memory,
  uint8_t *cpu_type,
  Symbols *symbols)
{
  FILE *in;

  memory->clear();

  uint32_t start = 0xffffffff;
  uint32_t end = 0xffffffff; 

  in = fopen(filename, "rb");

  if (in == NULL)
  {
    return -1;
  }

  memory->low_address = start;
  memory->high_address = end;

  fclose(in);

  return start;
}

