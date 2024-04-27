/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2024 by Michael Kohn
 *
 */

#ifndef NAKEN_ASM_TABLE_F8_H
#define NAKEN_ASM_TABLE_F8_H

#include <stdint.h>

enum
{
  F8_OP_NONE,
  F8_OP_DATA3,
  F8_OP_DATA4,
  F8_OP_DATA8,
  F8_OP_ADDR,
  F8_OP_DISP,
  F8_OP_DATA3_DISP,
  F8_OP_R,

  // These look the same as F8_OP_DATA4 / F8_OP_DATA8.
  //F8_OP_P4,
  //F8_OP_P8,

  F8_OP_SHIFT_1,
  F8_OP_SHIFT_4,

  F8_OP_A_DPCHR,
  F8_OP_DPCHR_A,

  F8_OP_A_IS,
  F8_OP_IS_A,

  F8_OP_A_R,
  F8_OP_R_A,

  F8_OP_H_DC0,
  F8_OP_Q_DC0,
  F8_OP_DC0_H,
  F8_OP_DC0_Q,
  F8_OP_K_PC1,
  F8_OP_PC1_K,
  F8_OP_PC0_Q,
  F8_OP_W_J,
  F8_OP_J_W,
};

struct _table_f8
{
  const char *instr;
  uint8_t opcode;
  uint8_t mask;
  uint8_t type;
  uint8_t cycles_min;
  uint8_t cycles_max;
};

extern struct _table_f8 table_f8[];

#endif

