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

#include "fileio/read_ti_txt.h"

enum
{
  TYPE_ADDRESS,
  TYPE_VALUE,
  TYPE_ERROR,
  TYPE_EOF,
};

int read_ti_txt(char *filename, struct _memory *memory)
{
FILE *in;
int ch;
int line = 1;
uint32_t address = 0;
uint32_t value;
uint32_t start, end;
int type;
int len;

  memory_clear(memory);

  start = 0xffffffff;
  end = 0;

  in = fopen(filename, "rb");
  if (in == 0)
  {
    return -1;
  }

  /* It's a state machine.. it's a state machine... */
  while(1)
  {
    type = TYPE_VALUE;
    value = 0;
    len = 0;

    while(1)
    {
      ch = getc(in);
      if (ch == '\r') { continue; }
      if (ch == '\n' || ch == ' ')
      {
        if (ch == '\n') { line++; }
        if (len == 0) { continue; }
        break;
      }
      if (ch == EOF)
      {
        if (len == 0) { type = TYPE_EOF; }
        break;
      }

      if (ch == '@') { type = TYPE_ADDRESS; continue; }
      if (ch == 'q') { type = TYPE_EOF; break; }

      if (ch >= '0' && ch <= '9') ch -= '0';
        else
      if (ch >= 'A' && ch <= 'F') ch = (ch - 'A') + 10;
        else
      if (ch >= 'a' && ch <= 'f') ch = (ch - 'a') + 10;
        else
      { type = TYPE_ERROR; break; }

      value = (value << 4) + ch;
      len++;
    }

#ifdef DEBUG
printf("%d) 0x%02x\n", type, value);
#endif

    if (type == TYPE_ADDRESS)
    {
      address = value;
    }
      else
    if (type == TYPE_VALUE)
    {
      if (address < start) { start = address; }
      if (address > end) { end = address; }

      memory_write_m(memory, address++, value);
    }
      else
    if (type == TYPE_ERROR)
    {
      printf("Error: Syntax error in TI TXT file on line %d\n", line);
      break;
    }
      else
    {
      break;
    }
  }

  fclose(in);

  memory->low_address = start;
  memory->high_address = end;

  return start;
}


