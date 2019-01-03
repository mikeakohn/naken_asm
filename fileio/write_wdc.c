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
#include "fileio/write_wdc.h"

int write_int24(FILE *out, int value)
{
  putc(value & 0xff, out);
  putc((value >> 8) & 0xff, out);
  putc((value >> 16) & 0xff, out);

  return 0;
}

int write_wdc(struct _memory *memory, FILE *out)
{
  uint32_t n;
  int address = -1;
  int length = 0;
  uint8_t buffer[65536];

  putc('Z', out);

  for (n = memory->low_address; n <= memory->high_address; n++)
  {
    if (memory_debug_line_m(memory, n) == DL_EMPTY || length == 65536)
    {
      if (length != 0)
      {
        write_int24(out, address);
        write_int24(out, length);
        fwrite(buffer, length, 1, out);

        length = 0;
        address = -1;
      }
    }
      else
    {
      if (address == -1) { address = n; }

      buffer[length++] = memory_read_m(memory, n);
    }
  }

  if (length != 0)
  {
    write_int24(out, address);
    write_int24(out, length);
    fwrite(buffer, length, 1, out);
  }

  return 0;
}

