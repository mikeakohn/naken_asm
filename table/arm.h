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

#ifndef NAKEN_ASM_TABLE_ARM_H
#define NAKEN_ASM_TABLE_ARM_H

#include <stdint.h>

#include "common/assembler.h"

enum
{
  OP_ALU_3,
  OP_ALU_2_N,
  OP_ALU_2_D,
  OP_MULTIPLY,
  OP_SWAP,
  OP_MRS,
  OP_MSR_ALL,
  OP_MSR_FLAG,
  OP_LDR_STR,
  OP_LDR_STR_HB,
  OP_UNDEFINED,
  OP_LDM_STM,
  OP_BRANCH,
  OP_BRANCH_EXCHANGE,
  OP_SWI,
  OP_CO_SWI,
  OP_CO_TRANSFER,
  OP_CO_OP_MASK,
  OP_CO_TRANSFER_MASK,
};

struct _table_arm
{
  char *instr;
  uint32_t opcode;
  uint32_t mask;
  uint8_t type;
  uint8_t len;
  int8_t cycles;
};

extern struct _table_arm table_arm[];

#endif

