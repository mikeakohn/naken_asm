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

#ifndef NAKEN_ASM_TABLE_PIC18_H
#define NAKEN_ASM_TABLE_PIC18_H

#include "common/assembler.h"

enum
{
  OP_NONE,
  OP_F_D_A,
  OP_F_A,
  OP_FS_FD,
  OP_F_B_A,
  OP_BRANCH_8,
  OP_BRANCH_11,
  OP_CALL,
  OP_GOTO,
  OP_S,
  OP_K8,
  OP_F_K12,
  OP_K4,
};

struct _table_pic18
{
  const char *instr;
  uint16_t opcode;
  uint16_t mask;
  uint8_t type;
  uint8_t default_a;
  int8_t cycles_min;
  int8_t cycles_max;
};

extern struct _table_pic18 table_pic18[];

#endif

