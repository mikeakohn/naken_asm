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

#include "fileio/read_srec.h"

// FIXME: Redundant with read_hex.c
static int get_hex(FILE *in, int len)
{
  int ch;
  int n = 0;

  while(len > 0)
  {
    ch = getc(in);
    if (ch == EOF) return -1;
    if (ch >= '0' && ch <= '9') ch -= '0';
      else
    if (ch >= 'A' && ch <= 'F') ch = (ch - 'A') + 10;
      else
    if (ch >= 'a' && ch <= 'f') ch = (ch - 'a') + 10;
      else
    { return -2; }

    n = (n << 4) + ch;
    len--;
  }

  return n;
}

static void ignore_line(FILE *in)
{
  int ch;

  while(1)
  {
    ch = getc(in);
    if (ch == '\n' || ch == EOF) { break; }
  }
}

int read_srec(char *filename, struct _memory *memory)
{
  FILE *in;
  int ch;
  int byte_count;
  int address;
  int record_type;
  int checksum;
  int checksum_calc;
  int n;
  int start_address = 0;
  int line = 0;
  int start, end;

  memory_clear(memory);

  start = -1;
  end = -1;

  in = fopen(filename, "rb");

  if (in == 0)
  {
    return -1;
  }

  while(1)
  {
    line++;
    ch = getc(in);
    if (ch == EOF) break;

    // If line doesn't start with S, ignore the line (this is a bad file maybe)
    if (ch != 'S')
    {
      ignore_line(in);
      continue;
    }

    record_type = getc(in);

    if (record_type >= '0' && record_type <= '9')
    {
      record_type -= '0';
    }
      else
    {
      record_type = 10;
    }

    // SREC's header has no data and ignore any headers with no data
    if (record_type == 0 || record_type > 3)
    {
      ignore_line(in);
      continue;
    }

    byte_count = get_hex(in, 2);

    checksum_calc = byte_count;

    if (record_type == 1)
    {
      address = get_hex(in, 4);
      checksum_calc = byte_count + (address >> 8) + (address & 0xff);
      byte_count -= 3;
    }
      else
    if (record_type == 2)
    {
      address = get_hex(in, 6);
      checksum_calc = byte_count + (address >> 16) + ((address >> 8) & 0xff) + (address & 0xff);
      byte_count -= 4;
    }
      else
    {
      address = get_hex(in, 8);
      checksum_calc = byte_count + (address >> 24) + ((address >> 16) & 0xff) + ((address >> 8) & 0xff) + (address & 0xff);
      byte_count -= 5;
    }

#ifdef DEBUG1
    printf("record_type: %02x (%d)\n", record_type, record_type);
    printf(" data_bytes: %02x (%d)\n", byte_count, byte_count);
    printf("    address: %04x (%d)\n", address, address);
#endif

    if (start == -1)
    {
      start = address;
      end = address + byte_count - 1;
    }
      else
    {
      if (address < start) { start = address; }
      if (address + byte_count > end) { end = address + byte_count - 1; }
    }

    for (n = 0; n < byte_count; n++)
    {
      ch = get_hex(in, 2);
      memory_write_m(memory, address++, ch);
      checksum_calc += ch;
#ifdef DEBUG1
      printf(" %02x",ch);
#endif
    }

#ifdef DEBUG1
    printf("\n");
#endif

    checksum = get_hex(in, 2);
    checksum_calc = ((checksum_calc & 0xff) ^ 0xff) & 0xff;

#ifdef DEBUG1
    printf("   checksum: %02x [%02x]\n\n",checksum, checksum_calc);
#endif

    if (checksum != checksum_calc)
    {
      printf("read_srec: Checksum failure on line %d!\n", line);
      start_address = -4;
      break;
    }

    // Remove anything at the end of the line.
    ignore_line(in);
  }

  if (in != NULL)  { fclose(in); }

  memory->low_address = start;
  memory->high_address = end;

  return start_address;
}

