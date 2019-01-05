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
};

struct _table_dotnet
{
  const char *instr;
  uint8_t opcode;
  uint8_t type;
};

extern struct _table_dotnet table_dotnet[];

#endif

