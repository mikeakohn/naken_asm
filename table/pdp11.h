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
  OP_DOUBLE,
  OP_D_EXTRA,
  OP_SINGLE,
  OP_BRANCH,
  OP_SUB_BR,
  OP_JSR,
  OP_REG,
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

