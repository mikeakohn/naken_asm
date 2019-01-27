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

#ifndef NAKEN_ASM_TABLE_XTENSA_H
#define NAKEN_ASM_TABLE_XTENSA_H

#include "common/assembler.h"

enum
{
  XTENSA_OP_NONE,
  XTENSA_OP_N_NONE,
  XTENSA_OP_AR_AT,
  XTENSA_OP_FR_FS,
  XTENSA_OP_FR_FS_FT,
  XTENSA_OP_AR_AS_AT,
  XTENSA_OP_AT_AS_I8,
  XTENSA_OP_AT_AS_IM8,
  XTENSA_OP_N_AR_AS_AT,
  XTENSA_OP_N_AR_AS_I4,
  XTENSA_OP_BT_BS4,
  XTENSA_OP_BT_BS8,
  XTENSA_OP_BR_BS_BT,
  XTENSA_OP_BRANCH_AS_AT_I8,
  XTENSA_OP_BRANCH_B5_I8,
  XTENSA_OP_BRANCH_AS_C4_I8,
  XTENSA_OP_BRANCH_AS_I12,
  XTENSA_OP_BRANCH_N_AS_I6,
  XTENSA_OP_BRANCH_BS_I8,
  XTENSA_OP_NUM_NUM,
  XTENSA_OP_N_NUM_NUM,
  XTENSA_OP_CALL_I18,
  XTENSA_OP_CALL_AS,
  XTENSA_OP_AR_FS_0_15,
  XTENSA_OP_AR_AS_7_22,
  XTENSA_OP_AS_0_1020,
  XTENSA_OP_AS_0_240,
  XTENSA_OP_AS_0_32760,
  XTENSA_OP_AR_AT_SHIFT_MASK,
  XTENSA_OP_FR_AS_0_15,
  XTENSA_OP_AT_AS_0_255,
  XTENSA_OP_AT_AS_0_510,
  XTENSA_OP_AT_AS_0_1020,
  XTENSA_OP_AT_AS_N64_N4,
  XTENSA_OP_N_AT_AS_0_60,
  XTENSA_OP_AT_I16,
};

struct _table_xtensa
{
  char *instr;
  uint32_t opcode_le;
  uint32_t opcode_be;
  uint8_t type;
};

struct _mask_xtensa
{
  uint32_t mask_le;
  uint32_t mask_be;
  uint8_t bits;
};

extern struct _table_xtensa table_xtensa[];
extern struct _mask_xtensa mask_xtensa[];
extern int xtensa_b4const[];
extern int xtensa_b4constu[];

#endif

