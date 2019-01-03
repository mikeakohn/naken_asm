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
#include <time.h>

#include "common/memory.h"
#include "common/cpu_list.h"
#include "fileio/write_srec.h"

#define LINE_LENGTH 16

static void write_srec_line(FILE *out, int type, uint32_t address, uint8_t *data, int len)
{
  int checksum = 0;
  int n;

  if (type == -1)
  {
    if (address <= 0xffff)
    {
      type = 1;
    }
      else
    if (address <= 0xffffff)
    {
      type = 2;
    }
      else
    {
      type = 3;
    }
  }

  if (type <= 1)
  {
    address &= 0xffff;
    fprintf(out, "S%c%02X%04X", '0' + type, len + 3, address);

    checksum = (len + 3) + (address >> 8) + (address & 0xff);
  }
    else
  if (type == 2)
  {
    address &= 0xffffff;
    fprintf(out, "S%c%02X%06X", '0' + type, len + 4, address);

    checksum = (len + 4) + (address >> 16) + ((address >> 24) & 0xff) +
      (address & 0xff);
  }
    else
  if (type == 3)
  {
    fprintf(out, "S%c%02X%08X", '0' + type, len + 5, address);

    checksum = (len + 5) + (address >> 24) + ((address >> 16) & 0xff) +
      ((address >> 8) & 0xff) + (address & 0xff);
  }

  for (n = 0; n < len; n++)
  {
    fprintf(out, "%02X", data[n]);

    checksum += data[n];
  }

  fprintf(out, "%02X\n", (((checksum & 0xff) ^ 0xff)));
}

// Encode an int so the hex value looks like the original int.
// Example: 24 will be 0x24
static uint8_t int_as_hex(int num)
{
  return ((((num / 10) << 4) + (num % 10)) & 0xff);
}

static void write_srec_header(FILE *out)
{
  time_t timestamp_sec;
  struct tm *timestamp;
  uint8_t data[7];

  timestamp_sec= time(NULL);
  timestamp = localtime(&timestamp_sec);

  timestamp->tm_year += 1900;

  data[0] = int_as_hex(timestamp->tm_year / 100);
  data[1] = int_as_hex(timestamp->tm_year % 100);
  data[2] = int_as_hex(timestamp->tm_mon);
  data[3] = int_as_hex(timestamp->tm_mday);
  data[4] = int_as_hex(timestamp->tm_hour);
  data[5] = int_as_hex(timestamp->tm_min);
  data[6] = int_as_hex(timestamp->tm_sec);

  write_srec_line(out, 0, 0, data, 7);
}

int write_srec(struct _memory *memory, FILE *out, int srec_size)
{
  uint8_t data[LINE_LENGTH];
  uint32_t address = 0;
  uint32_t n;
  int len, type;

  if (srec_size == SREC_24)
  {
    type = 2;
  }
    else
  if (srec_size == SREC_32)
  {
    type = 3;
  }
    else
  {
    type = -1;
  }

  write_srec_header(out);

  len = -1;

  for (n = memory->low_address; n <= memory->high_address; n++)
  {
    if (memory_debug_line_m(memory, n) == DL_EMPTY)
    {
      if (len > 0)
      {
        write_srec_line(out, type, address, data, len);
        len = -1;
      }

      continue;
    }

    if ((n & 0xffff) == 0 && len > 0)
    {
      write_srec_line(out, type, address, data, len);
      len = -1;
    }

    if (len == -1)
    {
      address = n;
      len = 0;
    }

    data[len++] = memory_read_m(memory, n);

    if (len == LINE_LENGTH)
    {
      write_srec_line(out, type, address, data, len);
      len = -1;
    }
  }

  if (len > 0)
  {
    write_srec_line(out, type, address, data, len);
  }

  if (memory->entry_point != 0xffffffff)
  {
    int checksum = 3 + ((memory->entry_point >> 8) & 0xff) +
                        (memory->entry_point & 0xff);

    checksum = (checksum & 0xff) ^ 0xff;

    fprintf(out, "S903%04x%02x\n", memory->entry_point, checksum);
  }


  return 0;
}

