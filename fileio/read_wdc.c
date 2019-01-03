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

#include "fileio/read_wdc.h"

int read_int24(FILE *in)
{
  int num;

  num = getc(in);

  if (num == EOF) { return 0; }

  num |= (getc(in) << 8);
  num |= (getc(in) << 16);

  return num;
}

int read_wdc(char *filename, struct _memory *memory)
{
  FILE *in;
  int ch, n;
  //int address = start_address;
  memory->low_address = 0xffffffff;

  memory_clear(memory);

  in = fopen(filename, "rb");
  if (in == 0)
  {
    return -1;
  }

  ch = getc(in);

  if (ch != 'Z')
  {
    return -1;
  }

  while(1)
  {
    int address = read_int24(in);
    int length = read_int24(in);

    if (length == 0) { break; }

    if (address < memory->low_address)
    {
      memory->low_address = address;
    }

    for (n = 0; n < length; n++)
    {
      ch = getc(in);

      if (ch == EOF) break;

      if (address > memory->high_address)
      {
        memory->high_address = address;
      }

      memory_write_m(memory, address++, ch);
    }
  }

  if (in != NULL)
  {
    fclose(in);
  }

  return memory->low_address;
}


