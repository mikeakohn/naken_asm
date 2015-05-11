/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2015 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fileio/read_hex.h"

int read_bin(char *filename, struct _memory *memory, uint32_t start_address)
{
  FILE *in;
  int ch;
  int address = start_address;

  memory_clear(memory);

  in = fopen(filename, "rb");
  if (in == 0)
  {
    return -1;
  }

  while(1)
  {
    ch = getc(in);
    if (ch == EOF) break;

    memory_write_m(memory, address++, ch);
  }

  if (in != NULL)
  {
    fclose(in);
  }

  memory->low_address = start_address;
  memory->high_address = address - 1;

  return start_address;
}


