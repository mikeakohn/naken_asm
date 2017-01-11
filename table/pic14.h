/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2017 by Michael Kohn
 *
 */

#ifndef _TABLE_PIC14_H
#define _TABLE_PIC14_H

#include "common/assembler.h"

enum
{
  OP_NONE,
  OP_F_D,
  OP_F,
  OP_F_B,
  OP_K8,
  OP_K11,
};

struct _table_pic14
{
  char *instr;
  uint16_t opcode;
  uint16_t mask;
  uint8_t type;
  uint8_t cycles_min;
  uint8_t cycles_max;
};

extern struct _table_pic14 table_pic14[];

#endif


