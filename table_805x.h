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

#ifndef _TABLE_805X_H
#define _TABLE_805X_H

#include "assembler.h"

enum
{
  OP_NONE,
  OP_PAGE,
  OP_REG,
  OP_AT_REG,
  OP_A,
  OP_DATA,
  OP_IRAM_ADDR,
  OP_C,
  OP_BIT_ADDR,
  OP_SLASH_BIT_ADDR,
  OP_RELADDR,
  OP_AB,
  OP_DPTR,
  OP_AT_A_PLUS_DPTR,
  OP_CODE_ADDR,
  OP_DATA_16,
  OP_AT_A_PLUS_PC,
  OP_AT_DPTR
};

struct _table_805x
{
  char *name;
  char op1;
  char op2;
  char op3;
  char range;   // range of register or page (r0, r1, r2 etc)
};

extern struct _table_805x table_805x[];

#endif


