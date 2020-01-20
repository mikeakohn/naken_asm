/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2020 by Michael Kohn
 *
 */

#ifndef NAKEN_ASM_TABLE_MCS48_H
#define NAKEN_ASM_TABLE_MCS48_H

#include "common/assembler.h"

enum
{
  OP_NONE,
  OP_A,
  OP_C,
  OP_I,
  OP_T,
  OP_F0,
  OP_F1,
  OP_BUS,
  OP_CLK,
  OP_CNT,
  OP_MBK,
  OP_RBK,
  OP_PSW,
  OP_TCNT,
  OP_TCNTI,
  OP_AT_A,
  OP_PP,
  OP_P0,
  OP_P03,
  OP_P12,
  OP_RR,
  OP_AT_R,
  OP_NUM,
  OP_ADDR,
  OP_PADDR,
};

struct _table_mcs48
{
  char *name;
  uint8_t opcode;
  uint8_t mask;
  uint8_t operand_count;
  uint8_t operand_1;
  uint8_t operand_2;
  uint8_t cycles;
};

extern struct _table_mcs48 table_mcs48[];

#endif

