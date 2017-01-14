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

#ifndef _TABLE_SUPER_FX_H
#define _TABLE_SUPER_FX_H

#include "common/assembler.h"

enum
{
  OP_NONE,
  OP_REG,      // Rn
  OP_ATREG,    // (Rn)
  OP_N,        // #nybble
  //OP_XX,
  OP_OFFSET,
  OP_REG_PP,   // Rn, #byte
  OP_REG_XX,   // Rn, #word
  OP_REG_ATXX, // Rn, (word)
  OP_REG_ATYY, // Rn, (byte)
  OP_ATXX_REG, // (word), Rn
  OP_ATYY_REG, // (byte), Rn
};

struct _table_super_fx
{
  char *instr;
  uint8_t opcode;
  uint8_t alt;
  uint8_t mask;
  uint8_t type;
  uint16_t reg_mask;
};

extern struct _table_super_fx table_super_fx[];

#endif


