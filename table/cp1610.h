/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2021 by Michael Kohn
 *
 */

#ifndef NAKEN_ASM_TABLE_CP1610_H
#define NAKEN_ASM_TABLE_CP1610_H

#include <stdint.h>

enum
{
  CP1610_OP_ILLEGAL,
  CP1610_OP_NONE,
  CP1610_OP_REG,
  CP1610_OP_SREG_DREG,
  CP1610_OP_AREG_SREG,
  CP1610_OP_IMMEDIATE_REG,
  CP1610_OP_REG_IMMEDIATE,
  CP1610_OP_ADDRESS_REG,
  CP1610_OP_REG_ADDRESS,
  CP1610_OP_1OP,
  CP1610_OP_BRANCH,
  CP1610_OP_JUMP,
  CP1610_OP_JSR,
  CP1610_OP_JR,
  CP1610_OP_REG_X2,
};

struct _table_cp1610
{
  const char *instr;
  uint16_t opcode;
  uint16_t mask;
  uint8_t type;
  uint8_t cycles_min;
  uint8_t cycles_max;
  //uint8_t words;
};

struct _table_cp1610_jump
{
  const char *instr;
  uint8_t bb;
  uint8_t ii;
  uint8_t use_reg;
};

extern struct _table_cp1610 table_cp1610[];
extern struct _table_cp1610_jump table_cp1610_jump[];

#endif

