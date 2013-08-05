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

#include "table_thumb.h"

struct _table_thumb table_thumb[] =
{
  { "lsl", 0x0000, 0xf800, OP_SHIFT, 2 },
  { "lsr", 0x0800, 0xf800, OP_SHIFT, 2 },
  { "asr", 0x1000, 0xf800, OP_SHIFT, 2 },
  { "add", 0x1800, 0xfa00, OP_ADD_SUB, 2 },
  { "sub", 0x1a00, 0xfa00, OP_ADD_SUB, 2 },
  { "mov", 0x2000, 0xf800, OP_IMM, 2 },
  { "cmp", 0x2800, 0xf800, OP_IMM, 2 },
  { "add", 0x3000, 0xf800, OP_IMM, 2 },
  { "sub", 0x3800, 0xf800, OP_IMM, 2 },
  { "and", 0x4000, 0xffc0, OP_ALU, 2 },
  { "eor", 0x4040, 0xffc0, OP_ALU, 2 },
  { "lsl", 0x4080, 0xffc0, OP_ALU, 2 },
  { "lsr", 0x40c0, 0xffc0, OP_ALU, 2 },
  { "asr", 0x4100, 0xffc0, OP_ALU, 2 },
  { "adc", 0x4140, 0xffc0, OP_ALU, 2 },
  { "sbc", 0x4180, 0xffc0, OP_ALU, 2 },
  { "ror", 0x41c0, 0xffc0, OP_ALU, 2 },
  { "tst", 0x4200, 0xffc0, OP_ALU, 2 },
  { "neg", 0x4240, 0xffc0, OP_ALU, 2 },
  { "cmp", 0x4280, 0xffc0, OP_ALU, 2 },
  { "cmn", 0x42c0, 0xffc0, OP_ALU, 2 },
  { "orr", 0x4300, 0xffc0, OP_ALU, 2 },
  { "mul", 0x4340, 0xffc0, OP_ALU, 2 },
  { "bic", 0x4380, 0xffc0, OP_ALU, 2 },
  { "mvn", 0x43c0, 0xffc0, OP_ALU, 2 },
  { "add", 0x4400, 0xff00, OP_HI, 2 },
  { "cmp", 0x4500, 0xff00, OP_HI, 2 },
  { "mov", 0x4600, 0xff00, OP_HI, 2 },
  { "bx", 0x4700, 0xff00, OP_HI_BX, 2 },  // This is odd.
  { NULL, 0, 0, 0, 0 }
};



