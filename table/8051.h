/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2022 by Michael Kohn
 *
 */

#ifndef NAKEN_ASM_TABLE_8051_H
#define NAKEN_ASM_TABLE_8051_H

#include "common/assembler.h"

enum
{
  OP_NONE,
  OP_REG,
  OP_AT_REG,
  OP_A,
  OP_C,
  OP_AB,
  OP_DPTR,
  OP_AT_A_PLUS_DPTR,
  OP_AT_A_PLUS_PC,
  OP_AT_DPTR,
  OP_DATA,
  OP_DATA_16,
  OP_CODE_ADDR,
  OP_SLASH_BIT_ADDR,
  OP_BIT_ADDR,
  OP_PAGE,
  OP_RELADDR,
  OP_IRAM_ADDR,
};

struct _table_8051
{
  char *name;
  char op[3];
  char range;   // range of register or page (r0, r1, r2 etc)
};

extern struct _table_8051 table_8051[];

#endif

