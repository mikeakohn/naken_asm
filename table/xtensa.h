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
  XTENSA_REG_AR = 1,
  XTENSA_REG_FR = 2,
  XTENSA_REG_BR = 3,
  XTENSA_REG_MW = 4,
};

enum
{
  XTENSA_OP_NONE,
  XTENSA_OP_N_NONE,
  XTENSA_OP_AR_AT,
  XTENSA_OP_AT_AS,
  XTENSA_OP_FR_FS,
  XTENSA_OP_FR_FS_FT,
  XTENSA_OP_AR_AS_AT,
  XTENSA_OP_AT_AS_N128_127,
  XTENSA_OP_AT_AS_N32768_32512,
  XTENSA_OP_N_AR_AS_AT,
  XTENSA_OP_N_AR_AS_N1_15,
  XTENSA_OP_BT_BS4,
  XTENSA_OP_BT_BS8,
  XTENSA_OP_BR_BS_BT,
  XTENSA_OP_BRANCH_AS_AT_I8,
  XTENSA_OP_BRANCH_AS_B5_I8,
  XTENSA_OP_BRANCH_AS_B5_I8_L,
  XTENSA_OP_BRANCH_AS_C4_I8,
  XTENSA_OP_BRANCH_AS_I12,
  XTENSA_OP_BRANCH_N_AS_I6,
  XTENSA_OP_BRANCH_BS_I8,
  XTENSA_OP_NUM_NUM,
  XTENSA_OP_N_NUM,
  XTENSA_OP_CALL_I18,
  XTENSA_OP_JUMP_I18,
  XTENSA_OP_AS,
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
  XTENSA_OP_MW_AS,
  XTENSA_OP_LOOP_AS_LABEL,
  XTENSA_OP_FT_AS_0_1020,
  XTENSA_OP_FR_AS_AT,
  XTENSA_OP_AR_AS_AS,
  XTENSA_OP_N_AT_AS,
  XTENSA_OP_FR_FS_AT,
  XTENSA_OP_AR_AS_BT,
  XTENSA_OP_FR_FS_BT,
  XTENSA_OP_AT_N2048_2047,
  XTENSA_OP_N_AS_N2048_2047,
  XTENSA_OP_AS_AT,
  XTENSA_OP_AS_MY,
  XTENSA_OP_MX_AT,
  XTENSA_OP_MX_MY,
  XTENSA_OP_MW_AS_MX_AT,
  XTENSA_OP_MW_AS_MX_MY,
  XTENSA_OP_BR_FS_FT,
  XTENSA_OP_0_15,
  XTENSA_OP_AR_FS,
  XTENSA_OP_N8_7,
  XTENSA_OP_AT_0_15,
  XTENSA_OP_AT_SPR,
  XTENSA_OP_AR_UR,
  XTENSA_OP_AR_AS,
  XTENSA_OP_AR_AS_1_31,
  XTENSA_OP_AR_AT_1_31,
  XTENSA_OP_AR_AT_0_15,
  XTENSA_OP_0_31,
  XTENSA_OP_FR_AS,
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
  uint8_t reg_0;
  uint8_t reg_1;
  uint8_t reg_2;
};

extern struct _table_xtensa table_xtensa[];
extern struct _mask_xtensa mask_xtensa[];
extern int xtensa_b4const[];
extern int xtensa_b4constu[];

#endif

