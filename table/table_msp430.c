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
  { "jne", 0x2000, 0xfc00, OP_JUMP },
  { "jeq", 0x2400, 0xfc00, OP_JUMP },
  { "jlo", 0x2800, 0xfc00, OP_JUMP },
  { "jhs", 0x2c00, 0xfc00, OP_JUMP },
  { "jn", 0x3000, 0xfc00, OP_JUMP },
  { "jge", 0x3400, 0xfc00, OP_JUMP },
  { "jl", 0x3800, 0xfc00, OP_JUMP },
  { "jmp", 0x3c00, 0xfc00, OP_JUMP },
  { "mov", 0x4000, 0xf000, OP_TWO_OPERAND },
  { "add", 0x5000, 0xf000, OP_TWO_OPERAND },
  { "addc", 0x6000, 0xf000, OP_TWO_OPERAND },
  { "subc", 0x7000, 0xf000, OP_TWO_OPERAND },
  { "sub", 0x8000, 0xf000, OP_TWO_OPERAND },
  { "cmp", 0x9000, 0xf000, OP_TWO_OPERAND },
  { "dadd", 0xa000, 0xf000, OP_TWO_OPERAND },
  { "bit", 0xb000, 0xf000, OP_TWO_OPERAND },
  { "bic", 0xc000, 0xf000, OP_TWO_OPERAND },
  { "bis", 0xd000, 0xf000, OP_TWO_OPERAND },
  { "sor", 0xe000, 0xf000, OP_TWO_OPERAND },
  { "and", 0xf000, 0xf000, OP_TWO_OPERAND },
  { "mova", 0x0000, 0xf0f0, OP_MOVA_AT_REG_REG },
  { "mova", 0x0010, 0xf0f0, OP_MOVA_AT_REG_PLUS_REG },
  { "mova", 0x0020, 0xf0f0, OP_MOVA_ABS20_REG },
  { "mova", 0x0030, 0xf0f0, OP_MOVA_INDIRECT_REG },
  { "rrcm", 0x0040, 0xf3e0, OP_SHIFT20 },
  { "rram", 0x0140, 0xf3e0, OP_SHIFT20 },
  { "rlam", 0x0240, 0xf3e0, OP_SHIFT20 },
  { "rrum", 0x0340, 0xf3e0, OP_SHIFT20 },
  { "mova", 0x0060, 0xf0f0, OP_MOVA_REG_ABS },
  { "mova", 0x0070, 0xf0f0, OP_MOVA_REG_INDIRECT },
  { "mova", 0x0080, 0xf0f0, OP_IMMEDIATE_REG },
  { "cmpa", 0x0090, 0xf0f0, OP_IMMEDIATE_REG },
  { "adda", 0x00a0, 0xf0f0, OP_IMMEDIATE_REG },
  { "suba", 0x00b0, 0xf0f0, OP_IMMEDIATE_REG },
  { "mova", 0x00c0, 0xf0f0, OP_REG_REG },
  { "cmpa", 0x00d0, 0xf0f0, OP_REG_REG },
  { "adda", 0x00e0, 0xf0f0, OP_REG_REG },
  { "suba", 0x00f0, 0xf0f0, OP_REG_REG },
  { "calla", 0x1340, 0xffc0, OP_CALLA_SOURCE },
  { "calla", 0x1380, 0xfff0, OP_CALLA_ABS20 },
  { "calla", 0x1390, 0xfff0, OP_CALLA_INDIRECT_PC },
  { "calla", 0x13b0, 0xfff0, OP_CALLA_IMMEDIATE },
  { "pushm", 0x0000, 0xfe00, OP_PUSH },
  { "popm", 0x0000, 0xfe00, OP_POP },
  { NULL, 0, 0, 0 }
};

