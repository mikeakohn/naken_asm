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
  OP_DATA_16,
  OP_CODE_ADDR,
  OP_BIT_ADDR,
  OP_DATA,
  OP_PAGE,
  OP_RELADDR,
  OP_SLASH_BIT_ADDR,
  OP_IRAM_ADDR,
};

struct _table_8051
{
  char *name;
  char op[3];
  char range;   // range of register or page (r0, r1, r2 etc)
};

extern struct _table_8051 table_8051[];

struct _address_map
{
  const char *name;
  uint8_t address;
  uint8_t is_bit_addressable;
};

extern struct _address_map address_map[];

extern struct _address_map address_map_psw[];

#endif

