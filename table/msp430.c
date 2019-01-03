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

#include "table/msp430.h"

struct _table_msp430 table_msp430[] =
{
  { "rrc",   0x1000, 0xff80, OP_ONE_OPERAND, VERSION_MSP430 },
  { "swpb",  0x1080, 0xffc0, OP_ONE_OPERAND_W, VERSION_MSP430 },
  { "rra",   0x1100, 0xff80, OP_ONE_OPERAND, VERSION_MSP430 },
  { "sxt",   0x1180, 0xffc0, OP_ONE_OPERAND_W, VERSION_MSP430 },
  { "push",  0x1200, 0xff80, OP_ONE_OPERAND, VERSION_MSP430 },
  { "call",  0x1280, 0xffc0, OP_ONE_OPERAND_X, VERSION_MSP430 },
  { "reti",  0x1300, 0xffff, OP_NONE, VERSION_MSP430 },
  { "jne",   0x2000, 0xfc00, OP_JUMP, VERSION_MSP430 },
  { "jnz",   0x2000, 0xfc00, OP_JUMP, VERSION_MSP430 },
  { "jeq",   0x2400, 0xfc00, OP_JUMP, VERSION_MSP430 },
  { "jz",    0x2400, 0xfc00, OP_JUMP, VERSION_MSP430 },
  { "jlo",   0x2800, 0xfc00, OP_JUMP, VERSION_MSP430 },
  { "jnc",   0x2800, 0xfc00, OP_JUMP, VERSION_MSP430 },
  { "jhs",   0x2c00, 0xfc00, OP_JUMP, VERSION_MSP430 },
  { "jc",    0x2c00, 0xfc00, OP_JUMP, VERSION_MSP430 },
  { "jn",    0x3000, 0xfc00, OP_JUMP, VERSION_MSP430 },
  { "jge",   0x3400, 0xfc00, OP_JUMP, VERSION_MSP430 },
  { "jl",    0x3800, 0xfc00, OP_JUMP, VERSION_MSP430 },
  { "jmp",   0x3c00, 0xfc00, OP_JUMP, VERSION_MSP430 },
  { "mov",   0x4000, 0xf000, OP_TWO_OPERAND, VERSION_MSP430 },
  { "add",   0x5000, 0xf000, OP_TWO_OPERAND, VERSION_MSP430 },
  { "addc",  0x6000, 0xf000, OP_TWO_OPERAND, VERSION_MSP430 },
  { "subc",  0x7000, 0xf000, OP_TWO_OPERAND, VERSION_MSP430 },
  { "sub",   0x8000, 0xf000, OP_TWO_OPERAND, VERSION_MSP430 },
  { "cmp",   0x9000, 0xf000, OP_TWO_OPERAND, VERSION_MSP430 },
  { "dadd",  0xa000, 0xf000, OP_TWO_OPERAND, VERSION_MSP430 },
  { "bit",   0xb000, 0xf000, OP_TWO_OPERAND, VERSION_MSP430 },
  { "bic",   0xc000, 0xf000, OP_TWO_OPERAND, VERSION_MSP430 },
  { "bis",   0xd000, 0xf000, OP_TWO_OPERAND, VERSION_MSP430 },
  { "xor",   0xe000, 0xf000, OP_TWO_OPERAND, VERSION_MSP430 },
  { "and",   0xf000, 0xf000, OP_TWO_OPERAND, VERSION_MSP430 },
  { "mova",  0x0000, 0xf0f0, OP_MOVA_AT_REG_REG, VERSION_MSP430X },
  { "mova",  0x0010, 0xf0f0, OP_MOVA_AT_REG_PLUS_REG, VERSION_MSP430X },
  { "mova",  0x0020, 0xf0f0, OP_MOVA_ABS20_REG, VERSION_MSP430X },
  { "mova",  0x0030, 0xf0f0, OP_MOVA_INDEXED_REG, VERSION_MSP430X },
  { "rrcm",  0x0040, 0xf3e0, OP_SHIFT20, VERSION_MSP430X },
  { "rram",  0x0140, 0xf3e0, OP_SHIFT20, VERSION_MSP430X },
  { "rlam",  0x0240, 0xf3e0, OP_SHIFT20, VERSION_MSP430X },
  { "rrum",  0x0340, 0xf3e0, OP_SHIFT20, VERSION_MSP430X },
  { "mova",  0x0060, 0xf0f0, OP_MOVA_REG_ABS, VERSION_MSP430X },
  { "mova",  0x0070, 0xf0f0, OP_MOVA_REG_INDEXED, VERSION_MSP430X },
  { "mova",  0x0080, 0xf0f0, OP_IMMEDIATE_REG, VERSION_MSP430X },
  { "cmpa",  0x0090, 0xf0f0, OP_IMMEDIATE_REG, VERSION_MSP430X },
  { "adda",  0x00a0, 0xf0f0, OP_IMMEDIATE_REG, VERSION_MSP430X },
  { "suba",  0x00b0, 0xf0f0, OP_IMMEDIATE_REG, VERSION_MSP430X },
  { "mova",  0x00c0, 0xf0f0, OP_REG_REG, VERSION_MSP430X },
  { "cmpa",  0x00d0, 0xf0f0, OP_REG_REG, VERSION_MSP430X },
  { "adda",  0x00e0, 0xf0f0, OP_REG_REG, VERSION_MSP430X },
  { "suba",  0x00f0, 0xf0f0, OP_REG_REG, VERSION_MSP430X },
  { "calla", 0x1340, 0xffc0, OP_CALLA_SOURCE, VERSION_MSP430X },
  { "calla", 0x1380, 0xfff0, OP_CALLA_ABS20, VERSION_MSP430X },
  { "calla", 0x1390, 0xfff0, OP_CALLA_INDIRECT_PC, VERSION_MSP430X },
  { "calla", 0x13b0, 0xfff0, OP_CALLA_IMMEDIATE, VERSION_MSP430X },
  { "pushm", 0x1400, 0xfe00, OP_PUSH, VERSION_MSP430X },
  { "popm",  0x1600, 0xfe00, OP_POP, VERSION_MSP430X },
  { "rrcx",  0x1000, 0xff80, OP_X_ONE_OPERAND, VERSION_MSP430X_EXT },
  { "swpbx", 0x1080, 0xffc0, OP_X_ONE_OPERAND, VERSION_MSP430X_EXT },
  { "rrax",  0x1100, 0xff80, OP_X_ONE_OPERAND, VERSION_MSP430X_EXT },
  { "sxtx",  0x1180, 0xffc0, OP_X_ONE_OPERAND, VERSION_MSP430X_EXT },
  { "pushx", 0x1200, 0xff80, OP_X_ONE_OPERAND, VERSION_MSP430X_EXT },
  { "callx", 0x1280, 0xffc0, OP_X_ONE_OPERAND, VERSION_MSP430X_EXT },
  { "movx",  0x4000, 0xf000, OP_X_TWO_OPERAND, VERSION_MSP430X_EXT },
  { "addx",  0x5000, 0xf000, OP_X_TWO_OPERAND, VERSION_MSP430X_EXT },
  { "addcx", 0x6000, 0xf000, OP_X_TWO_OPERAND, VERSION_MSP430X_EXT },
  { "subcx", 0x7000, 0xf000, OP_X_TWO_OPERAND, VERSION_MSP430X_EXT },
  { "subx",  0x8000, 0xf000, OP_X_TWO_OPERAND, VERSION_MSP430X_EXT },
  { "cmpx",  0x9000, 0xf000, OP_X_TWO_OPERAND, VERSION_MSP430X_EXT },
  { "daddx", 0xa000, 0xf000, OP_X_TWO_OPERAND, VERSION_MSP430X_EXT },
  { "bitx",  0xb000, 0xf000, OP_X_TWO_OPERAND, VERSION_MSP430X_EXT },
  { "bicx",  0xc000, 0xf000, OP_X_TWO_OPERAND, VERSION_MSP430X_EXT },
  { "bisx",  0xd000, 0xf000, OP_X_TWO_OPERAND, VERSION_MSP430X_EXT },
  { "xorx",  0xe000, 0xf000, OP_X_TWO_OPERAND, VERSION_MSP430X_EXT },
  { "andx",  0xf000, 0xf000, OP_X_TWO_OPERAND, VERSION_MSP430X_EXT },
  { NULL, 0, 0, 0 }
};

