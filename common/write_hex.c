/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2012 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "assembler.h"
#include "lookup_tables.h"
#include "write_hex.h"

static void write_hex_line(FILE *out, int address, unsigned char *data, int len, int *segment)
{
int checksum;
int n;

  if (address<(*segment) || address>(*segment+0xffff))
  {
    *segment=address&0xffff0;
    checksum=2+(((*segment)>>12)&0xff)+(((*segment)>>4)&0xff)+2;
    fprintf(out, ":02000002%02X%02X%02X\n", ((*segment)>>12)&0xff, ((*segment)>>4)&0xff, (((checksum&0xff)^0xff)+1)&0xff);
  }

//printf("%05x %05x\n", address, *segment);
  address=address-(*segment);

  fprintf(out, ":%02X%04X00", len, address);
  checksum=len+(address>>8)+(address&255);

  for (n=0; n<len; n++)
  {
    fprintf(out, "%02X", data[n]);
    checksum=checksum+data[n];
  }

  fprintf(out,"%02X\n", (((checksum&0xff)^0xff)+1)&0xff);
}

int write_hex(struct _asm_context *asm_context, FILE *out)
{
unsigned char data[16];
int len;
int n;
int address=0,segment=0;

  len=-1;
  for (n=asm_context->memory.low_address; n<=asm_context->memory.high_address; n++)
  {
    if (memory_debug_line(asm_context, n)==DL_EMPTY)
    {
      if (len>0)
      {
        write_hex_line(out, address, data, len, &segment);
        len=-1;
      }

      continue;
    }

    if ((n&0x0ffff)==0 && len>0)
    {
      write_hex_line(out, address, data, len, &segment);
      len=-1;
    }

    if (len==-1)
    {
      address=n;
      len=0;
    }

    data[len++]=memory_read(asm_context, n);

    if (len==16)
    {
      write_hex_line(out, address, data, len, &segment);
      len=-1;
    }
  }

  if (len>0)
  {
    write_hex_line(out, address, data, len, &segment);
  }

  fputs(":00000001FF\n", out);

  return 0;
}

int write_bin(struct _asm_context *asm_context, FILE *out)
{
int n;

  for (n=asm_context->memory.low_address; n<=asm_context->memory.high_address; n++)
  {
    putc(memory_read(asm_context, n), out);
  }

  return 0;
}

