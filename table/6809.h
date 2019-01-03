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

#ifndef NAKEN_ASM_TABLE_6809_H
#define NAKEN_ASM_TABLE_6809_H

#include <stdint.h>

enum
{
  M6809_OP_ILLEGAL,
  M6809_OP_INHERENT,
  M6809_OP_IMMEDIATE,
  M6809_OP_EXTENDED,
  M6809_OP_RELATIVE,
  M6809_OP_LONG_RELATIVE,
  M6809_OP_DIRECT,
  M6809_OP_STACK,
  M6809_OP_TWO_REG,
  M6809_OP_INDEXED,
};

struct _table_6809
{
  const char *instr;
  uint16_t opcode;
  uint8_t operand_type;
  uint8_t bytes;
  uint8_t cycles_min;
  uint8_t cycles_max;
};

extern struct _table_6809 table_6809[];
extern struct _table_6809 table_6809_16[];

#endif

