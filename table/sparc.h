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

#ifndef NAKEN_ASM_TABLE_SPARC_H
#define NAKEN_ASM_TABLE_SPARC_H

#include "common/assembler.h"

enum
{
  OP_NONE,
  OP_REG_REG_REG,
  OP_REG_SIMM13_REG,
  OP_FREG_FREG_FREG_FREG,
  OP_FREG_FREG_IMM5_FREG,
  OP_FREG_FREG_FREG,
  OP_BRANCH,
  OP_BRANCH_P,
  OP_BRANCH_P_REG,
  OP_CALL,
  OP_IMM_ASI_REG_REG,
  OP_ASI_REG_REG,
};

struct _table_sparc
{
  char *instr;
  uint32_t opcode;
  uint8_t type;
  uint8_t flags;
};

extern struct _table_sparc table_sparc[];
extern uint32_t mask_sparc[];

#endif

