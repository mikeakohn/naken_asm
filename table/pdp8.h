/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2020 by Michael Kohn, Lars Brinkhoff
 *
 * PDP-8 by Lars Brinkhoff
 *
 */

#ifndef NAKEN_ASM_TABLE_PDP8_H
#define NAKEN_ASM_TABLE_PDP8_H

#include "common/assembler.h"

enum
{
  OP_NONE,
  OP_M,
  OP_IOT,
  OP_OPR,
};

struct _table_pdp8
{
  char *instr;
  uint16_t opcode;
  uint16_t mask;
  uint8_t type;
  //uint8_t cycles_min;
  //uint8_t cycles_max;
};

extern struct _table_pdp8 table_pdp8[];

#endif

