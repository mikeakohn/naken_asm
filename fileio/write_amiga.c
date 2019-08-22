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
#include "fileio/amiga_hunk.h"
#include "fileio/write_amiga.h"

static void write_uint32(FILE *out, uint32_t value)
{
  putc((value >> 24) & 0xff, out);
  putc((value >> 16) & 0xff, out);
  putc((value >> 8) & 0xff, out);
  putc(value & 0xff, out);
}

int write_amiga(struct _memory *memory, FILE *out)
{
  uint32_t n;
  uint32_t length = (memory->high_address + 1) - memory->low_address;

  // Hunk file header.
  write_uint32(out, HUNK_HEADER); // magic_cookie
  write_uint32(out, 0x00000000);  // name_length
  write_uint32(out, 0x00000001);  // table_length
  write_uint32(out, 0x00000000);  // first_hunk
  write_uint32(out, 0x00000000);  // last_hunk
  write_uint32(out, length / 4);  // length of code

  // Hunk code.
  write_uint32(out, HUNK_CODE);   // hunk_code
  write_uint32(out, length / 4);  // length of code

  for (n = memory->low_address; n <= memory->high_address; n++)
  {
    putc(memory_read_m(memory, n), out);
  }

  // Hunk end.
  write_uint32(out, HUNK_END);    // hunk_end

  return 0;
}

