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

#ifndef NAKEN_ASM_TABLE_MCS48_H
#define NAKEN_ASM_TABLE_MCS48_H

#include "common/assembler.h"

enum
{
  OP_NONE,
  OP_A,
  OP_ADDR,
  OP_AT_A,
  OP_AT_R,
  OP_BUS,
  OP_C,
  OP_CLK,
  OP_CNT,
  OP_F0,
  OP_F1,
  OP_I,
  OP_MBK,
  OP_NUM,
  OP_PADDR,
  OP_PP,
  OP_PSW,
  OP_RBK,
  OP_RR,
  OP_T,
  OP_TCNT,
  OP_TCNTI,
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

