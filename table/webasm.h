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
};

struct _table_webasm
{
  const char *instr;
  uint8_t opcode;
  uint8_t type;
};

extern struct _table_webasm table_webasm[];

#endif

