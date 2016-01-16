/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2016 by Michael Kohn
 *
 */

#ifndef _TABLE_6809_H
#define _TABLE_6809_H

#include <stdint.h>

enum
{
  M6809_OP_ILLEGAL,
  M6809_OP_INHERENT,
  M6809_OP_IMMEDIATE,
  M6809_OP_EXTENDED,
  M6809_OP_RELATIVE,
  M6809_OP_DIRECT,
  M6809_OP_INDEXED,
};

struct _m6809_table
{
  const char *instr;
  uint16_t opcode;
  uint8_t operand_type;
  uint8_t bytes;
  uint8_t cycles_min;
  uint8_t cycles_max;
};

extern struct _m6809_table m6809_table[];
extern struct _m6809_table m6809_table_16[];

#endif

