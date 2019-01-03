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

#ifndef NAKEN_ASM_TABLE_SWEET16_H
#define NAKEN_ASM_TABLE_SWEET16_H

#include <stdint.h>

enum
{
  SWEET16_OP_ILLEGAL,
  SWEET16_OP_NONE,
  SWEET16_OP_REG,
  SWEET16_OP_AT_REG,
  SWEET16_OP_EA,
  SWEET16_OP_REG_VALUE,
};

struct _table_sweet16
{
  const char *instr;
  uint8_t opcode;
  uint8_t mask;
  uint8_t type;
};

extern struct _table_sweet16 table_sweet16[];

#endif

