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

#ifndef NAKEN_ASM_TABLE_PDK15_H
#define NAKEN_ASM_TABLE_PDK15_H

#include "common/assembler.h"

enum
{
  OP_NONE,
  OP_A,
  OP_IO_A,
  OP_A_IO,
  OP_K8,
  OP_M7,
  OP_M8,
  OP_A_M8,
  OP_M8_A,
  OP_A_M7,
  OP_M7_A,
  OP_IO_N,
  OP_M_N,
  OP_A_K,
  OP_K12,
};

struct _table_pdk15
{
  char *instr;
  uint16_t opcode;
  uint16_t mask;
  uint8_t type;
  uint8_t cycles_min;
  uint8_t cycles_max;
};

extern struct _table_pdk15 table_pdk15[];

#endif

