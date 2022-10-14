/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2022 by Michael Kohn
 *
 */

#ifndef NAKEN_ASM_TABLE_PDK16_H
#define NAKEN_ASM_TABLE_PDK16_H

#include "common/assembler.h"

enum
{
  OP_NONE,
  OP_A,
  OP_K5,
  OP_N4,
  OP_IO_A,
  OP_A_IO,
  OP_M8,
  OP_K8,
  OP_A_M9,
  OP_M9_A,
  OP_A_M8,
  OP_M8_A,
  OP_A_K,
  OP_M9,
  OP_IO_N,
  OP_M_N,
  OP_K13,
};

struct _table_pdk16
{
  char *instr;
  uint16_t opcode;
  uint16_t mask;
  uint8_t type;
  uint8_t cycles_min;
  uint8_t cycles_max;
};

extern struct _table_pdk16 table_pdk16[];

#endif

