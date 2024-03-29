/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2023 by Michael Kohn
 *
 */

#ifndef NAKEN_ASM_TABLE_LC3_H
#define NAKEN_ASM_TABLE_LC3_H

#include "common/assembler.h"

enum
{
  OP_NONE,
  OP_R_R_R,
  OP_R_R_IMM5,
  OP_BR,
  OP_BASER,
  OP_OFFSET11,
  OP_R_OFFSET9,
  OP_R_R_OFFSET6,
  OP_R_R,
  OP_VECTOR,
};

struct _table_lc3
{
  const char *instr;
  uint16_t opcode;
  uint16_t mask;
  uint8_t type;
};

extern struct _table_lc3 table_lc3[];

#endif

