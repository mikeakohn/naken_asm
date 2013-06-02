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

#include "table_msp430.h"

struct _table_msp430 table_msp430[] =
{
  { "rrc", 0x1000, 0xff80, OP_ONE_OPERAND },
  { "swpb", 0x1080, 0xff80, OP_ONE_OPERAND },
  { "rra", 0x1000, 0xff80, OP_ONE_OPERAND },
  { "sxt", 0x1080, 0xff80, OP_ONE_OPERAND },
  { "push", 0x1000, 0xff80, OP_ONE_OPERAND },
  { "call", 0x1080, 0xff80, OP_ONE_OPERAND },
  { "reti", 0x1000, 0xffff, OP_NONE },
  { "jne", 0x1000, 0xfc00, OP_JUMP },
  { "jeq", 0x1000, 0xfc00, OP_JUMP },
  { "jnc", 0x1000, 0xfc00, OP_JUMP },
  { "jhs", 0x1000, 0xfc00, OP_JUMP },
  { "jn", 0x1000, 0xfc00, OP_JUMP },
  { "jge", 0x1000, 0xfc00, OP_JUMP },
  { "jl", 0x1000, 0xfc00, OP_JUMP },
  { "jmp", 0x1000, 0xfc00, OP_JUMP },


  { NULL, 0, 0, 0 }
};

