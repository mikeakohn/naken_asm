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

#ifndef NAKEN_ASM_TABLE_PDK13_H
#define NAKEN_ASM_TABLE_PDK13_H

#include "common/assembler.h"

enum
{
  OP_NONE,
  OP_A,
  OP_IO_A,
  OP_A_IO,
  OP_M,
  OP_M4,
  OP_K8,
  OP_A_M,
  OP_M_A,
  OP_A_M4,
  OP_M4_A,
  OP_IO_N,
  OP_M_N,
  OP_A_K,
  OP_K10,
};

struct _table_pdk13
{
  char *instr;
  uint16_t opcode;
  uint16_t mask;
  uint8_t type;
  uint8_t cycles_min;
  uint8_t cycles_max;
};

extern struct _table_pdk13 table_pdk13[];

#endif

