/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2022 by Michael Kohn
 *
 */

#ifndef NAKEN_ASM_TABLE_ARC_H
#define NAKEN_ASM_TABLE_ARC_H

#include "common/assembler.h"

enum
{
  OP_NONE,
  OP_B_C,
  OP_A_B_C,
  OP_B_B_C,
  OP_B_B_H,
  OP_B_B_LIMM,
  OP_B_B_U,
  OP_B_SP_U7,
  OP_B_B_U7,
  OP_B_B_U5,
  OP_B_U5,
  OP_B_LIMM,
  OP_C_B_U3,
  OP_SP_SP_U7,
  OP_R0_GP_S9,
  OP_B_H,
  OP_B_U7,
  OP_B_U8,
  OP_HOB,
};

#define F_F  1
#define F_CC 2
#define F_D 3

struct _table_arc
{
  char *instr;
  uint32_t opcode;
  uint32_t mask;
  uint8_t type;
  uint8_t flags;
};

struct _table_arc16
{
  char *instr;
  uint16_t opcode;
  uint16_t mask;
  uint8_t type;
};

struct _table_arc_op
{
  char *instr;
  uint8_t opcode;
};

// FIXME: Delete these.
extern struct _table_arc table_arc[];
extern struct _table_arc16 table_arc16[];

extern struct _table_arc_op table_arc_alu[];
extern struct _table_arc_op table_arc_alu16[];
extern struct _table_arc_op table_arc_single[];
extern struct _table_arc_op table_arc_special[];
extern struct _table_arc_op table_arc_alu32[];

#endif

