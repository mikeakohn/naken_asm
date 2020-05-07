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

#ifndef NAKEN_ASM_TABLE_TMS340_H
#define NAKEN_ASM_TABLE_TMS340_H

#include <stdint.h>

#include "common/assembler.h"

enum
{
  OP_NONE,
  OP_REG,
  OP_P_REG,
  OP_P_REG_DISP,
  OP_P_REG_P,
  OP_P_REG_XY,
  OP_MP_REG,
  OP_ADDRESS,
  OP_AT_ADDR,
  OP_LIST,
  OP_B,
  OP_F,
  OP_K,
  OP_L,
  OP_N,
  OP_Z,
  OP_FE,
  OP_FS,
  OP_IL,
  OP_IW,
  OP_NN,
  OP_XY,
};

struct _table_tms340
{
  char *instr;
  uint16_t opcode;
  uint16_t mask;
  uint8_t operand_count;
  uint8_t operand_types[3];
};

extern struct _table_tms340 table_tms340[];

#endif

