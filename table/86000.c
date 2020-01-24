/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2020 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "table/86000.h"

struct _table_86000 table_86000[] =
{
  { "addc",  0x92, 0xfe, OP_ADDRESS,                    1 },
  { "addc",  0x91, 0xff, OP_IMMEDIATE,                  1 },
  { "addc",  0x94, 0xfc, OP_AT_REG,                     1 },
  { "add",   0x82, 0xfe, OP_ADDRESS,                    1 },
  { "add",   0x81, 0xff, OP_IMMEDIATE,                  1 },
  { "add",   0x84, 0xfc, OP_AT_REG,                     1 },
  { "and",   0xe2, 0xfe, OP_ADDRESS,                    1 },
  { "and",   0xe1, 0xff, OP_IMMEDIATE,                  1 },
  { "and",   0xe4, 0xfc, OP_AT_REG,                     1 },
  { "be",    0x32, 0xfe, OP_ADDRESS_RELATIVE8,          2 },
  { "be",    0x31, 0xff, OP_IMMEDIATE_RELATIVE8,        2 },
  { "be",    0x34, 0xfc, OP_AT_REG_IMMEDIATE_RELATIVE8, 2 },
  { "bn",    0x88, 0xe8, OP_ADDRESS_BIT_RELATIVE8,      2 },
  { "bne",   0x42, 0xfe, OP_ADDRESS_RELATIVE8,          2 },
  { "bne",   0x41, 0xff, OP_IMMEDIATE_RELATIVE8,        2 },
  { "bne",   0x44, 0xfc, OP_AT_REG_IMMEDIATE_RELATIVE8, 2 },
  { "bnz",   0x90, 0xff, OP_RELATIVE8,                  2 },
  { "bpc",   0x48, 0xe8, OP_ADDRESS_BIT_RELATIVE8,      2 },
  { "bp",    0x68, 0xe8, OP_ADDRESS_BIT_RELATIVE8,      2 },
  { "brf",   0x11, 0xff, OP_RELATIVE16,                 4 },
  { "br",    0x01, 0xff, OP_RELATIVE8,                  2 },
  { "bz",    0x80, 0xff, OP_RELATIVE8,                  2 },
  { "call",  0x08, 0xe8, OP_ADDRESS12,                  2 },
  { "callf", 0x20, 0xff, OP_ADDRESS16,                  2 },
  { "callr", 0x10, 0xff, OP_RELATIVE16,                 4 },
  { "clr1",  0xc8, 0xe8, OP_ADDRESS_BIT,                1 },
  { "dbnz",  0x52, 0xfe, OP_ADDRESS_RELATIVE8,          2 },
  { "dbnz",  0x54, 0xfc, OP_AT_REG_RELATIVE8,           2 },
  { "dec",   0x72, 0xfe, OP_ADDRESS,                    1 },
  { "dec",   0x74, 0xfc, OP_AT_REG,                     1 },
  { "div",   0x40, 0xff, OP_NONE,                       7 },
  { "inc",   0x62, 0xfe, OP_ADDRESS,                    1 },
  { "inc",   0x64, 0xfc, OP_AT_REG,                     1 },
  { "jmp",   0x28, 0xe8, OP_ADDRESS12,                  2 },
  { "jmpf",  0x21, 0xff, OP_ADDRESS16,                  2 },
  { "ldc",   0xc1, 0xff, OP_NONE,                       2 },
  { "ld",    0x02, 0xfe, OP_ADDRESS,                    1 },
  { "ld",    0x04, 0xfc, OP_AT_REG,                     1 },
  { "mov",   0x22, 0xfe, OP_IMMEDIATE_ADDRESS,          2 },
  { "mov",   0x24, 0xfc, OP_IMMEDIATE_AT_REG,           1 },
  { "mul",   0x30, 0xff, OP_NONE,                       7 },
  { "nop",   0x00, 0xff, OP_NONE,                       1 },
  { "not1",  0xa8, 0xe8, OP_ADDRESS_BIT,                1 },
  { "or",    0xd2, 0xfe, OP_ADDRESS,                    1 },
  { "or",    0xd1, 0xff, OP_IMMEDIATE,                  1 },
  { "or",    0xd4, 0xfc, OP_AT_REG,                     1 },
  { "pop",   0x70, 0xfe, OP_ADDRESS,                    2 },
  { "push",  0x60, 0xfe, OP_ADDRESS,                    2 },
  { "ret",   0xa0, 0xff, OP_NONE,                       2 },
  { "reti",  0xb0, 0xff, OP_NONE,                       2 },
  { "rol",   0xe0, 0xff, OP_NONE,                       1 },
  { "rolc",  0xf0, 0xff, OP_NONE,                       1 },
  { "ror",   0xc0, 0xff, OP_NONE,                       1 },
  { "rorc",  0xd0, 0xff, OP_NONE,                       1 },
  { "set1",  0xe8, 0xe8, OP_ADDRESS_BIT,                1 },
  { "st",    0x12, 0xfe, OP_ADDRESS,                    1 },
  { "st",    0x14, 0xfc, OP_AT_REG,                     1 },
  { "subc",  0xb2, 0xfe, OP_ADDRESS,                    1 },
  { "subc",  0xb1, 0xff, OP_IMMEDIATE,                  1 },
  { "subc",  0xb4, 0xfc, OP_AT_REG,                     1 },
  { "sub",   0xa2, 0xfe, OP_ADDRESS,                    1 },
  { "sub",   0xa1, 0xff, OP_IMMEDIATE,                  1 },
  { "sub",   0xa4, 0xfc, OP_AT_REG,                     1 },
  { "xch",   0xc2, 0xfe, OP_ADDRESS,                    1 },
  { "xch",   0xc4, 0xfc, OP_AT_REG,                     1 },
  { "xor",   0xf2, 0xfe, OP_ADDRESS,                    1 },
  { "xor",   0xf1, 0xff, OP_IMMEDIATE,                  1 },
  { "xor",   0xf4, 0xfc, OP_AT_REG,                     1 },
  { NULL,    0x00, 0x00, OP_NONE,                       0 },
};

