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

#include "fileio/read_hex.h"

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

int read_hex(char *filename, struct _memory *memory)
{
  FILE *in;
  int ch;
  int byte_count;
  int address;
  int record_type;
  int checksum;
  int checksum_calc;
  int n;
  int start_address=0;
  int line=0;
  int start, end;
  int segment=0;

  //memset(memory, 0, memory_len);
  memory_clear(memory);
  //memset(dirty, 0, memory->size);

  start = -1;
  end = -1;

  in = fopen(filename, "rb");
  if (in == 0)
  {
    return -1;
  }

  /* It's a state machine.. it's a state machine... */
  while(1)
  {
    line++;
    ch = getc(in);
    if (ch == EOF) break;

    if (ch != ':')
    {
      /* Line is a junkie piece of shit because ch says so */
      while(1) { if (ch == '\n' || ch == EOF) break; ch = getc(in); }
      continue;
    }

    byte_count = get_hex(in,2);
    address = get_hex(in,4);
    record_type = get_hex(in,2);
    checksum_calc = byte_count + (address&0xff) + (address>>8) + record_type;

#ifdef DEBUG1
    printf(" byte_count: %02x (%d)\n",byte_count,byte_count);
    printf("    address: %04x (%d)\n",address,address);
    printf("record_type: %02x (%d)\n",record_type,record_type);
#endif

    switch(record_type) 
    {
      /* Data Record */
      case 0x00:
        address += segment;

        if (start == -1)
        {
          start = address;
          end = address + byte_count - 1;
        }
          else
        {
          if (address < start) start = address;
          if (address + byte_count > end) end = address + byte_count - 1;
        }

        for (n = 0; n < byte_count; n++)
        {
          ch = get_hex(in, 2);
          //dirty[address]=1;
          memory_write_m(memory, address++, ch);
          checksum_calc += ch;
#ifdef DEBUG1
          printf(" %02x",ch);
#endif
        }
        break;

      /* End Of File */
      case 0x01:
        start_address = get_hex(in, byte_count<<1);
        break;

      /* Extended Segment Address Record */
      case 0x02:
        ch = get_hex(in, 4);
        segment = (ch << 4);
        checksum_calc += (ch&0xff) + (ch>>8);
        #ifdef DEBUG1
        printf("Address %d\n", start_address);
        #endif
        break;

      /* Extended Linear Address Record */
      case 0x04:
        ch = get_hex(in, 4);
        checksum_calc += (ch&0xff) + (ch>>8);
        segment = (ch<<16);
        break;

      /* Start Segment Address Record */
      case 0x03:

      /* Start Linear Address Record */
      case 0x05:

      default:
        for (n = 0; n < byte_count; n++)
        {
          ch = get_hex(in, 2);
          checksum_calc += ch;
#ifdef DEBUG1
          printf(" %02x", ch);
#endif
        }
        //printf("Unsupported or unknown code: %d\n",record_type);
        break;
    }

    #ifdef DEBUG1
    printf("\n");
    #endif

    checksum = get_hex(in, 2);
    checksum_calc = (((checksum_calc & 0xff) ^ 0xff) + 1) & 0xff;

    #ifdef DEBUG1
    printf("   checksum: %02x [%02x]\n\n",checksum, checksum_calc);
    #endif

    if (checksum != checksum_calc)
    {
      printf("read_hex: Checksum failure on line %d!\n", line);
      fclose(in);
      in = NULL;
      start_address = -4;
      break;
    }

    /* All tied up to a state machine */
    while(1)
    {
      ch = getc(in);
      if (ch == '\n' || ch == EOF) break;
    }
  }

  /* We're all slaves to a state machine */
  if (in != NULL)
  {
    fclose(in);
  }

  memory->low_address = start;
  memory->high_address = end;

  return start_address;
}


