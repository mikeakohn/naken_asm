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

#ifndef NAKEN_ASM_TABLE_RISCV_H
#define NAKEN_ASM_TABLE_RISCV_H

#include "common/assembler.h"

enum
{
  OP_NONE,
  OP_R_TYPE,
  OP_R_R,
  OP_I_TYPE,
  OP_UI_TYPE,
  OP_SB_TYPE,
  OP_U_TYPE,
  OP_UJ_TYPE,
  OP_SHIFT,
  OP_FENCE,
  OP_FFFF,
  OP_READ,
  OP_RD_INDEX_R,
  OP_FD_INDEX_R,
  OP_RS_INDEX_R,
  OP_FS_INDEX_R,
  OP_LR,
  OP_STD_EXT,
  OP_R_FP,
  OP_R_FP_RM,
  OP_R_FP_FP,
  OP_FP,
  OP_FP_FP,
  OP_FP_FP_RM,
  OP_FP_R,
  OP_FP_R_RM,
  OP_FP_FP_FP,
  OP_FP_FP_FP_RM,
  OP_FP_FP_FP_FP_RM,
};

struct _table_riscv
{
  char *instr;
  uint32_t opcode;
  uint32_t mask;
  uint8_t type;
  uint8_t cycles_min;
  uint8_t cycles_max;
};

extern struct _table_riscv table_riscv[];

#endif


