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
  OP_A,
  OP_B,
  OP_C,
  OP_H,
  OP_LIMM,
  OP_U3,
  OP_U5,
  OP_U7,
  OP_U8,
  OP_S11,
  OP_R0,
  OP_GP,
  OP_SP,

  OP_A_PAREN_B_S9,
  OP_C_PAREN_B_S9,

  OP_A_PAREN_B_C,
  OP_B_PAREN_PCL_U10,
  OP_B_PAREN_SP_U7,
  OP_C_PAREN_B_U5,
  OP_C_PAREN_B_U6,
  OP_C_PAREN_B_U7,
  OP_R0_PAREN_GP_S10,
  OP_R0_PAREN_GP_S11,
  OP_R0_PAREN_GP_S9,
};

//#define F_F  1
//#define F_CC 2
//#define F_D 3

#define FLAG_NONE 0
#define FLAG_ZERO 1

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
  uint8_t operand_count;
  uint8_t operands[3];
};

struct _table_arc_op
{
  char *instr;
  uint8_t opcode;
  uint8_t is_extended;
};

// FIXME: Delete these.
extern struct _table_arc table_arc[];
extern struct _table_arc16 table_arc16[];

extern struct _table_arc_op table_arc_alu[];
extern struct _table_arc_op table_arc_alu16[];
extern struct _table_arc table_arc_load_store[];
extern struct _table_arc table_arc_load_store16[];

extern struct _table_arc_op table_arc_single[];
//extern struct _table_arc_op table_arc_special[];

#endif

