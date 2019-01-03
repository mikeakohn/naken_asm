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
#include "fileio/write_hex.h"

static void write_hex_line(FILE *out, uint32_t address, uint8_t *data, int len, uint32_t *segment)
{
  int checksum;
  int n;

#if 0
  // This was using sections of type 2.. let's change to type 4 (linear)

  if (address < (*segment) || address > (*segment + 0xffff))
  {
    *segment = address & 0xffff0;
    checksum = 2 + (((*segment)>>12)&0xff) + (((*segment)>>4)&0xff) + 2;
    fprintf(out, ":02000002%02X%02X%02X\n", ((*segment)>>12)&0xff, ((*segment)>>4)&0xff, (((checksum&0xff)^0xff)+1)&0xff);
  }

  address = address - (*segment);
#endif

  // Check if we should change the linear address (upper 16 bits of a possible
  // 32 bit address.
  if ((address & 0xffff0000) != *segment)
  {
    *segment = address & 0xffff0000;
//printf("address=%x segment=%x %02x %02x\n", address, *segment, (((*segment)>>24)&0xff),(((*segment)>>16)&0xff));
    checksum = 4 + (((*segment) >> 24) & 0xff) + (((*segment) >> 16) & 0xff) + 2;

    fprintf(out, ":02000004%04X%02X\n",
      ((*segment) >> 16) & 0xffff,
      (((checksum & 0xff) ^ 0xff) + 1) & 0xff);
  }

  address = address & 0xffff;

  // Ready to write data
  fprintf(out, ":%02X%04X00", len, address);
  checksum = len + (address >> 8) + (address & 255);

  for (n = 0; n < len; n++)
  {
    fprintf(out, "%02X", data[n]);
    checksum = checksum + data[n];
  }

  fprintf(out,"%02X\n", (((checksum & 0xff) ^ 0xff) + 1) & 0xff);
}

// NOTE: (to self) - This is kind of slowish.  I wonder if it would be
// better to request pages from the memory module and just dump the memory
// pages instead of treating it as one big memory thing.
int write_hex(struct _memory *memory, FILE *out)
{
  uint8_t data[16];
  int len;
  uint32_t n;
  uint32_t address = 0,segment = 0;

  //memory_dump(memory);

  len = -1;
  for (n = memory->low_address; n <= memory->high_address; n++)
  {
    if (memory_debug_line_m(memory, n) == DL_EMPTY)
    {
      if (len > 0)
      {
        write_hex_line(out, address, data, len, &segment);
        len = -1;
      }

      continue;
    }

    if ((n & 0x0ffff) == 0 && len > 0)
    {
      write_hex_line(out, address, data, len, &segment);
      len = -1;
    }

    if (len == -1)
    {
      address = n;
      len = 0;
    }

    data[len++] = memory_read_m(memory, n);

    if (len == 16)
    {
      write_hex_line(out, address, data, len, &segment);
      len = -1;
    }
  }

  if (len > 0)
  {
    write_hex_line(out, address, data, len, &segment);
  }

  fputs(":00000001FF\n", out);

  return 0;
}

