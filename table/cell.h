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

#ifndef NAKEN_ASM_TABLE_CELL_H
#define NAKEN_ASM_TABLE_CELL_H

#include "common/assembler.h"

enum
{
  OP_NONE,
  OP_RT_S10_RA,
  OP_RT_RA_S10,
  OP_RT_RA_U10,
  OP_RT_RA_U7,
  OP_RT_RA_S6,
  OP_RT_RA,
  OP_RA_RB,
  OP_RA,
  OP_RT,
  OP_RT_RA_RB,
  OP_RA_RB_RC,
  OP_RT_RA_RB_RC,
  OP_RT_ADDRESS,
  OP_RT_RELATIVE,
  OP_RT_S16,
  OP_RT_U16,
  OP_RT_U18,
  OP_RT_S7_RA,
  OP_RA_S10,
  OP_BRANCH_RELATIVE,
  OP_BRANCH_ABSOLUTE,
  OP_BRANCH_RELATIVE_RT,
  //OP_BRANCH_ABSOLUTE_RT,
  OP_HINT_RELATIVE_RO_RA,
  OP_HINT_ABSOLUTE_RO_I16,
  OP_HINT_RELATIVE_RO_I16,
  OP_RT_RA_SCALE155,
  OP_RT_RA_SCALE173,
  OP_U14,
  OP_RT_SA,
  OP_RT_CA,
  OP_SA_RT,
  OP_CA_RT,
};

struct _table_cell
{
  char *instr;
  uint32_t opcode;
  uint32_t mask;
  uint8_t type;
  uint8_t flags;
  uint8_t cycles;
  uint8_t cycles_max;
};

#define FLAG_NONE 0

extern struct _table_cell table_cell[];

#endif


