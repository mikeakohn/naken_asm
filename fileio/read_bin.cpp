/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2023 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fileio/read_bin.h"

int read_bin(const char *filename, Memory *memory, uint32_t start_address)
{
  FILE *in;
  int ch;
  uint32_t address = start_address;

  memory->clear();

  in = fopen(filename, "rb");

  if (in == NULL)
  {
    return -1;
  }

  while(1)
  {
    ch = getc(in);
    if (ch == EOF) break;

    memory->write8(address++, ch);
  }

  fclose(in);

  memory->low_address = start_address;
  memory->high_address = address - 1;

  return start_address;
}

