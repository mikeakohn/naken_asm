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

#include "fileio/amiga_hunk.h"
#include "fileio/read_amiga.h"

// FIXME: Is there another function in common that can do this?
static uint32_t read_int32(FILE *in)
{
  return (getc(in) << 24) |
         (getc(in) << 16) |
         (getc(in) << 8) |
          getc(in);
}

static long read_hunk_header(FILE *in)
{
  int ch, n;
  uint32_t name_length = read_int32(in);

  //printf("           Name: ");

  for (n = 0; n < name_length * 4; n++)
  {
    ch = getc(in);

    if (ch == 0) { continue; }
    if (ch == EOF) { break; }

    //printf("%c", ch);
  }

  //printf("\n");

  uint32_t table_length = read_int32(in);
  //uint32_t first_hunk = read_int32(in);
  //uint32_t last_hunk = read_int32(in);
  read_int32(in);
  read_int32(in);

  //printf("   Table Length: %d\n", table_length);
  //printf("     First Hunk: %d\n", first_hunk);
  //printf("      Last Hunk: %d\n", last_hunk);

  long table_offset = ftell(in);

  for (n = 0; n < table_length; n++)
  {
    //uint32_t size = read_int32(in);
    read_int32(in);

    //printf("         len=%d\n", size);
  }

  return table_offset;
}

static int read_code(FILE *in, struct _memory *memory)
{
  int n;
  uint32_t length = read_int32(in) * 4;

  for (n = 0; n < length; n++)
  {
    int ch = getc(in);
    if (ch == EOF) break;

    memory_write_m(memory, n, ch);
  }

  return length;
}

int read_amiga(char *filename, struct _memory *memory)
{
  FILE *in;

  memory_clear(memory);

  in = fopen(filename, "rb");

  if (in == NULL)
  {
    return -1;
  }

  uint32_t magic_cookie = read_int32(in);

  if (magic_cookie != HUNK_HEADER)
  {
    fclose(in);
    return -1;
  }

  fseek(in, 0, SEEK_SET);

  int running = 1;
  long table_offset = 0;
  int count = 0;
  int length = 0;

  while (running == 1)
  {
    uint32_t hunk_type = read_int32(in);

    long marker = ftell(in);

    if (table_offset != 0)
    {
      fseek(in, table_offset + count * 4, SEEK_SET);
      length = read_int32(in);
      fseek(in, marker, SEEK_SET);
    }

    switch (hunk_type)
    {
      case HUNK_HEADER:
        table_offset = read_hunk_header(in);
        break;
      case HUNK_CODE:
        read_code(in, memory);
        running = 0;
        break;
      default:
        if (length == 0)
        {
          fclose(in);
          return -1;
        }

        fseek(in, length, SEEK_CUR);

        break;
    }
  }

  fclose(in);

  return 0;
}

