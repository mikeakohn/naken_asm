/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2013 by Michael Kohn
 *
 */

#ifndef _TABLE_MSP430_H
#define _TABLE_MSP430_H

#include <stdint.h>

#include "assembler.h"

enum
{
  OP_NONE,
  OP_ONE_OPERAND,
  OP_JUMP,
  OP_TWO_OPERAND,
};

struct _table_msp430
{
  char *instr;
  uint16_t opcode;
  uint16_t mask;
  uint8_t type;
  //uint8_t cycles;
};

extern struct _table_msp430 table_msp430[];

#endif


