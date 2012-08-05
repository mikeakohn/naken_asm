/**
 *  naken_asm MSP430 assembler.
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
#include <ctype.h>

#include "asm_common.h"
#include "memory.h"

void add_bin(struct _asm_context *asm_context, unsigned short int b, int flags)
{
int line=DL_NO_CG;

  if (asm_context->pass==2 && flags==IS_OPCODE) { line=asm_context->line; }

  if (asm_context->memory.endian==ENDIAN_LITTLE)
  {
    // 1 little, 2 little, 3 little endian
    memory_write_inc(asm_context, b&0xff, line);
    memory_write_inc(asm_context, b>>8, DL_NO_CG);
  }
    else
  {
    memory_write_inc(asm_context, b>>8, DL_NO_CG);
    memory_write_inc(asm_context, b&0xff, line);
  }
}

void add_bin24(struct _asm_context *asm_context, unsigned int b, int flags)
{
int line=asm_context->line;

  if (asm_context->memory.endian==ENDIAN_LITTLE)
  {
    memory_write_inc(asm_context, b&0xff, line);
    memory_write_inc(asm_context, (b>>8)&0xff, line);
    memory_write_inc(asm_context, (b>>16)&0xff, line);
  }
    else
  {
    memory_write_inc(asm_context, (b>>16)&0xff, line);
    memory_write_inc(asm_context, (b>>8)&0xff, line);
    memory_write_inc(asm_context, b&0xff, line);
  }
}

void add_bin32(struct _asm_context *asm_context, unsigned int b, int flags)
{
int line=asm_context->line;

  if (asm_context->memory.endian==ENDIAN_LITTLE)
  {
    memory_write_inc(asm_context, b&0xff, line);
    memory_write_inc(asm_context, (b>>8)&0xff, line);
    memory_write_inc(asm_context, (b>>16)&0xff, line);
    memory_write_inc(asm_context, (b>>24)&0xff, line);
  }
    else
  {
    memory_write_inc(asm_context, (b>>24)&0xff, line);
    memory_write_inc(asm_context, (b>>16)&0xff, line);
    memory_write_inc(asm_context, (b>>8)&0xff, line);
    memory_write_inc(asm_context, b&0xff, line);
  }
}

int eat_operand(struct _asm_context *asm_context)
{
char token[TOKENLEN];
int token_type;

  // Eat all tokens until an ',' or EOL
  while(1)
  {
    token_type=get_token(asm_context, token, TOKENLEN);

    if (IS_TOKEN(token,',') || token_type==TOKEN_EOL)
    {
      pushback(asm_context, token, token_type);
      return 0;
    }
  }

  return -1;
}

void lower_copy(char *d, const char *s)
{
  while(1)
  {
    *d=tolower(*s);
    if (*s==0) break;
    d++;
    s++;
  }
}


