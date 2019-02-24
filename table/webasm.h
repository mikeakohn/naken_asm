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

#ifndef NAKEN_ASM_TABLE_WEBASM_H
#define NAKEN_ASM_TABLE_WEBASM_H

#include <stdint.h>

enum
{
  WEBASM_OP_ILLEGAL,
  WEBASM_OP_NONE,
  WEBASM_OP_UINT32,
  WEBASM_OP_UINT64,
  WEBASM_OP_VARINT64,
  WEBASM_OP_VARINT32,
  WEBASM_OP_FUNCTION_INDEX,
  WEBASM_OP_LOCAL_INDEX,
  WEBASM_OP_GLOBAL_INDEX,
  WEBASM_OP_BLOCK_TYPE,
  WEBASM_OP_RELATIVE_DEPTH,
  WEBASM_OP_TABLE,
  WEBASM_OP_INDIRECT,
  WEBASM_OP_MEMORY_IMMEDIATE,
};

struct _table_webasm
{
  const char *instr;
  uint8_t opcode;
  uint8_t type;
};

struct _webasm_types
{
  const char *name;
  uint8_t type;
};

extern struct _table_webasm table_webasm[];
extern struct _webasm_types webasm_types[];

#endif

