/**
 *  naken_asm MSP430 assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2023 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "asm/common.h"
#include "common/Memory.h"

void add_bin8(AsmContext *asm_context, uint8_t b, int flags)
{
  int line = DL_NO_CG;

  if (asm_context->pass == 2 && flags == IS_OPCODE)
  {
    line = asm_context->tokens.line;
  }

  if (asm_context->pass == 1 && asm_context->pass_1_write_disable == 1)
  {
    asm_context->address++;
    return;
  }

  asm_context->memory_write_inc(b, line);
}

void add_bin16(AsmContext *asm_context, uint16_t b, int flags)
{
  int line = DL_NO_CG;

  if (asm_context->pass == 2 && flags == IS_OPCODE)
  {
    line = asm_context->tokens.line;
  }

  if (asm_context->pass == 1 && asm_context->pass_1_write_disable == 1)
  {
    asm_context->address += 2;
    return;
  }

  if (asm_context->memory.endian == ENDIAN_LITTLE)
  {
    // 1 little, 2 little, 3 little endian
    asm_context->memory_write_inc(b & 0xff, line);
    asm_context->memory_write_inc(b >> 8, DL_NO_CG);
  }
    else
  {
    asm_context->memory_write_inc(b >> 8, DL_NO_CG);
    asm_context->memory_write_inc(b & 0xff, line);
  }
}

void add_bin32(AsmContext *asm_context, uint32_t b, int flags)
{
  int line = asm_context->tokens.line;

  if (asm_context->pass == 1 && asm_context->pass_1_write_disable == 1)
  {
    asm_context->address += 4;
    return;
  }

  if (asm_context->memory.endian == ENDIAN_LITTLE)
  {
    asm_context->memory_write_inc(b & 0xff, line);
    asm_context->memory_write_inc((b >> 8) & 0xff, line);
    asm_context->memory_write_inc((b >> 16) & 0xff, line);
    asm_context->memory_write_inc((b >> 24) & 0xff, line);
  }
    else
  {
    asm_context->memory_write_inc((b >> 24) & 0xff, line);
    asm_context->memory_write_inc((b >> 16) & 0xff, line);
    asm_context->memory_write_inc((b >> 8) & 0xff, line);
    asm_context->memory_write_inc(b & 0xff, line);
  }
}

int add_bin_varuint(AsmContext *asm_context, uint64_t b, int fixed_size)
{
  uint32_t num;
  int count = 0;
  int line = asm_context->tokens.line;

  if (asm_context->memory.endian == ENDIAN_BIG)
  {
    printf("Warning: varuint only works with little endian at %s:%d\n",
      asm_context->tokens.filename,
      asm_context->tokens.line);
  }

  while (1)
  {
    fixed_size--;

    num = b & 0x7f;
    b = b >> 7;

    if (b != 0 || fixed_size > 0)
    {
      num |= 0x80;
    }

    asm_context->memory_write_inc(num, line);

    count++;

    if (b == 0 && fixed_size <= 0) { break; }
  }

  return count;
}

int add_bin_varint(AsmContext *asm_context, uint64_t b, int fixed_size)
{
  uint32_t num;
  int count = 0;
  int line = asm_context->tokens.line;

  if (asm_context->memory.endian == ENDIAN_BIG)
  {
    printf("Warning: varint only works with little endian at %s:%d\n",
      asm_context->tokens.filename,
      asm_context->tokens.line);
  }

  while (1)
  {
    fixed_size--;

    num = b & 0x7f;
    b = b >> 7;

    if (b != 0 || fixed_size > 0)
    {
      num |= 0x80;
    }

    asm_context->memory_write_inc(num, line);

    count++;

    if (b == 0 && fixed_size <= 0) { break; }
  }

  return count;
}

