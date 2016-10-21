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

#ifndef _TABLE_POWERPC_H
#define _TABLE_POWERPC_H

#include "common/assembler.h"

enum
{
  OP_NONE,
  OP_R_R_R,
  OP_R_R_SIMM,
};

#define FLAG_NONE 0
#define FLAG_DOT 1
#define FLAG_REQUIRE_DOT 3

struct _table_powerpc
{
  char *instr;
  uint32_t opcode;
  uint32_t mask;
  uint8_t type;
  uint8_t flags;
  uint8_t cycles;
  uint8_t cycles_max;
};

extern struct _table_powerpc table_powerpc[];

#endif


