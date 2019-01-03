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

#ifndef NAKEN_ASM_TABLE_PS2_VU_H
#define NAKEN_ASM_TABLE_PS2_VU_H

#include <stdint.h>

enum
{
  EE_VU_OP_NONE,
  EE_VU_OP_FT,
  EE_VU_OP_FS,
  EE_VU_OP_FD,
  EE_VU_OP_VIT,
  EE_VU_OP_VIS,
  EE_VU_OP_VID,
  EE_VU_OP_VI01,
  EE_VU_OP_I,
  EE_VU_OP_Q,
  EE_VU_OP_P,
  EE_VU_OP_R,
  EE_VU_OP_ACC,
  EE_VU_OP_OFFSET,
  EE_VU_OP_OFFSET_BASE,
  EE_VU_OP_BASE,
  EE_VU_OP_BASE_DEC,
  EE_VU_OP_BASE_INC,
  EE_VU_OP_IMMEDIATE24,
  EE_VU_OP_IMMEDIATE15,
  EE_VU_OP_IMMEDIATE12,
  EE_VU_OP_IMMEDIATE5,
};

#define FLAG_NONE 0
#define FLAG_DEST 1
#define FLAG_BC 2
#define FLAG_VU1_ONLY 4
#define FLAG_XYZ 8
#define FLAG_TE 16
#define FLAG_SE 32

struct _table_ps2_ee_vu
{
  const char *instr;
  char operand[3];
  int8_t operand_count;
  uint32_t opcode;
  uint32_t mask;
  int8_t flags;
};

extern struct _table_ps2_ee_vu table_ps2_ee_vu_upper[];
extern struct _table_ps2_ee_vu table_ps2_ee_vu_lower[];

#endif

