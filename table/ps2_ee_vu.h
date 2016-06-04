/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2016 by Michael Kohn
 *
 */

#ifndef _TABLE_PS2_VU_H
#define _TABLE_PS2_VU_H

#include <stdint.h>

enum
{
  EE_VU_OP_NONE,
  EE_VU_OP_FT,
  EE_VU_OP_FS,
  EE_VU_OP_FD,
  EE_VU_OP_I,
  EE_VU_OP_Q,
};

#define FLAG_NONE 0
#define FLAG_DEST 1
#define FLAG_BC 2
#define FLAG_VU1_ONLY 4

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

