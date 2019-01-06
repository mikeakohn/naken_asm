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

#ifndef NAKEN_ASM_TABLE_DOTNET_H
#define NAKEN_ASM_TABLE_DOTNET_H

#include <stdint.h>

enum
{
  DOTNET_OP_ILLEGAL,
  DOTNET_OP_NONE,
  DOTNET_OP_UINT8,
  DOTNET_OP_UINT16,
  DOTNET_OP_INT8,
  DOTNET_OP_INT32,
  DOTNET_OP_INT64,
  DOTNET_OP_FLOAT32,
  DOTNET_OP_FLOAT64,
  DOTNET_OP_BRANCH8,
  DOTNET_OP_BRANCH32,
  DOTNET_OP_TYPE_TOK,
  DOTNET_OP_CALLSITEDESCR,
  DOTNET_OP_METHOD,
  DOTNET_OP_FIELD,
  DOTNET_OP_CLASS,
  DOTNET_OP_STRING,
  DOTNET_OP_TOKEN,
  DOTNET_OP_TYPE,
  DOTNET_OP_ETYPE,
  DOTNET_OP_CTOR,
  DOTNET_OP_PREFIX_CHECK,
  DOTNET_OP_PREFIX_ALIGNED,
  DOTNET_OP_THIS_TYPE,
  DOTNET_OP_VALUE_TYPE,
  DOTNET_OP_SWITCH,
};

struct _table_dotnet
{
  const char *instr;
  uint8_t opcode;
  uint8_t type;
};

extern struct _table_dotnet table_dotnet[];
extern struct _table_dotnet table_dotnet_fe[];

#endif

