/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2025 by Michael Kohn
 *
 */

#ifndef NAKEN_ASM_TABLE_PDP11_H
#define NAKEN_ASM_TABLE_PDP11_H

#include "common/assembler.h"

enum
{
  OP_NONE,
  OP_REG,
  OP_SINGLE,
  OP_DOUBLE,
  OP_REG_D,
  OP_REG_S,
  OP_BRANCH,
  OP_SUB_BR,
  OP_JSR,
  OP_NN,
  OP_S_OPER,
  OP_NZVC,
};

struct _table_pdp11
{
  const char *instr;
  uint16_t opcode;
  uint16_t mask;
  uint8_t type;
};

extern struct _table_pdp11 table_pdp11[];

#endif

