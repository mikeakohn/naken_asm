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

#include "fileio/read_bin.h"

int read_bin(char *filename, struct _memory *memory, uint32_t start_address)
{
  FILE *in;
  int ch;
  int address = start_address;

  memory_clear(memory);

  in = fopen(filename, "rb");

  if (in == NULL)
  {
    return -1;
  }

  while(1)
  {
    ch = getc(in);
    if (ch == EOF) break;

    memory_write_m(memory, address++, ch);
  }

  fclose(in);

  memory->low_address = start_address;
  memory->high_address = address - 1;

  return start_address;
}

