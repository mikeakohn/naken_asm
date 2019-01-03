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

#include <stdlib.h>

#include "table/propeller.h"

struct _table_propeller table_propeller[] = {
  { "wrbyte",  0x00000000, 0xfc800000, PROPELLER_OP_DS, 8, 23 },
  { "rdbyte",  0x00800000, 0xfc800000, PROPELLER_OP_DS, 8, 23 },
  { "wrword",  0x04000000, 0xfc800000, PROPELLER_OP_DS_15_1, 8, 23 },
  { "rdword",  0x04800000, 0xfc800000, PROPELLER_OP_DS_15_1, 8, 23 },
  { "wrlong",  0x08000000, 0xfc800000, PROPELLER_OP_DS_15_2, 8, 23 },
  { "rdlong",  0x08800000, 0xfc800000, PROPELLER_OP_DS_15_2, 8, 23 },
  { "clkset",  0x0c400000, 0xfc400007, PROPELLER_OP_D, 8, 23 },
  { "cogid",   0x0cc00001, 0xfc400007, PROPELLER_OP_D, 8, 23 },
  { "coginit", 0x0c400002, 0xfc400007, PROPELLER_OP_D, 8, 23 },
  { "cogstop", 0x0c400003, 0xfc400007, PROPELLER_OP_D, 8, 23 },
  { "locknew", 0x0cc00004, 0xfc400007, PROPELLER_OP_D, 8, 23 },
  { "lockret", 0x0c400005, 0xfc400007, PROPELLER_OP_D, 8, 23 },
  { "lockset", 0x0c400006, 0xfc400007, PROPELLER_OP_D, 8, 23 },
  { "lockclr", 0x0c400007, 0xfc400007, PROPELLER_OP_D, 8, 23 },
  { "hubop",   0x0c000000, 0xfc000000, PROPELLER_OP_DS, 8, 23 },
  { "mul",     0x10800000, 0xfc000000, PROPELLER_OP_DS, 0, 0 },
  { "muls",    0x14800000, 0xfc000000, PROPELLER_OP_DS, 0, 0 },
  { "enc",     0x18800000, 0xfc000000, PROPELLER_OP_DS, 0, 0 },
  { "ones",    0x1c800000, 0xfc000000, PROPELLER_OP_DS, 0, 0 },
  { "ror",     0x20800000, 0xfc000000, PROPELLER_OP_DS, 4, 4 },
  { "rol",     0x24800000, 0xfc000000, PROPELLER_OP_DS, 4, 4 },
  { "shr",     0x28800000, 0xfc000000, PROPELLER_OP_DS, 4, 4 },
  { "shl",     0x2c800000, 0xfc000000, PROPELLER_OP_DS, 4, 4 },
  { "rcr",     0x30800000, 0xfc000000, PROPELLER_OP_DS, 4, 4 },
  { "rcl",     0x34800000, 0xfc000000, PROPELLER_OP_DS, 4, 4 },
  { "sar",     0x38800000, 0xfc000000, PROPELLER_OP_DS, 4, 4 },
  { "rev",     0x3c800000, 0xfc000000, PROPELLER_OP_DS, 4, 4 },
  { "mins",    0x40800000, 0xfc000000, PROPELLER_OP_DS, 4, 4 },
  { "maxs",    0x44800000, 0xfc000000, PROPELLER_OP_DS, 4, 4 },
  { "min",     0x48800000, 0xfc000000, PROPELLER_OP_DS, 4, 4 },
  { "max",     0x4c800000, 0xfc000000, PROPELLER_OP_DS, 4, 4 },
  { "movs",    0x50800000, 0xfc000000, PROPELLER_OP_DS, 4, 4 },
  { "movd",    0x54800000, 0xfc000000, PROPELLER_OP_DS, 4, 4 },
  { "movi",    0x58800000, 0xfc000000, PROPELLER_OP_DS, 4, 4 },
  { "jmpret",  0x5c800000, 0xfc800000, PROPELLER_OP_DS, 4, 4 },
  { "jmp",     0x5c000000, 0xfc800000, PROPELLER_OP_S, 4, 4 },
  //{ "call",    0x5cc00000, 0xfc000000, PROPELLER_OP_IMMEDIATE, 4, 4 },
  { "call",    0x5cc00000, 0xfc000000, PROPELLER_OP_DS, 4, 4 },
  { "ret",     0x5c400000, 0xfc000000, PROPELLER_OP_NONE, 4, 4 },
  { "and",     0x60800000, 0xfc000000, PROPELLER_OP_DS, 4, 4 },
  { "andn",    0x64800000, 0xfc000000, PROPELLER_OP_DS, 4, 4 },
  { "test",    0x60000000, 0xfc800000, PROPELLER_OP_DS, 4, 4 },
  { "testn",   0x64000000, 0xfc800000, PROPELLER_OP_DS, 4, 4 },
  { "or",      0x68800000, 0xfc000000, PROPELLER_OP_DS, 4, 4 },
  { "xor",     0x6c800000, 0xfc000000, PROPELLER_OP_DS, 4, 4 },
  { "muxc",    0x70800000, 0xfc000000, PROPELLER_OP_DS, 4, 4 },
  { "muxnc",   0x74800000, 0xfc000000, PROPELLER_OP_DS, 4, 4 },
  { "muxz",    0x78800000, 0xfc000000, PROPELLER_OP_DS, 4, 4 },
  { "muxnz",   0x7c800000, 0xfc000000, PROPELLER_OP_DS, 4, 4 },
  { "add",     0x80800000, 0xfc000000, PROPELLER_OP_DS, 4, 4 },
  { "sub",     0x84800000, 0xfc000000, PROPELLER_OP_DS, 4, 4 },
  { "cmp",     0x84000000, 0xfc000000, PROPELLER_OP_DS, 4, 4 },
  { "addabs",  0x88800000, 0xfc000000, PROPELLER_OP_DS, 4, 4 },
  { "subabs",  0x8c800000, 0xfc000000, PROPELLER_OP_DS, 4, 4 },
  { "sumc",    0x90800000, 0xfc000000, PROPELLER_OP_DS, 4, 4 },
  { "sumnc",   0x94800000, 0xfc000000, PROPELLER_OP_DS, 4, 4 },
  { "sumz",    0x98800000, 0xfc000000, PROPELLER_OP_DS, 4, 4 },
  { "sumnz",   0x9c800000, 0xfc000000, PROPELLER_OP_DS, 4, 4 },
  { "mov",     0xa0800000, 0xfc000000, PROPELLER_OP_DS, 4, 4 },
  { "neg",     0xa4800000, 0xfc000000, PROPELLER_OP_DS, 4, 4 },
  { "abs",     0xa8800000, 0xfc000000, PROPELLER_OP_DS, 4, 4 },
  { "absneg",  0xac800000, 0xfc000000, PROPELLER_OP_DS, 4, 4 },
  { "negc",    0xb0800000, 0xfc000000, PROPELLER_OP_DS, 4, 4 },
  { "negnc",   0xb4800000, 0xfc000000, PROPELLER_OP_DS, 4, 4 },
  { "negz",    0xb8800000, 0xfc000000, PROPELLER_OP_DS, 4, 4 },
  { "negnz",   0xbc800000, 0xfc000000, PROPELLER_OP_DS, 4, 4 },
  { "cmps",    0xc0000000, 0xfc000000, PROPELLER_OP_DS, 4, 4 },
  { "cmpsx",   0xc4000000, 0xfc000000, PROPELLER_OP_DS, 4, 4 },
  { "addx",    0xc8800000, 0xfc000000, PROPELLER_OP_DS, 4, 4 },
  { "subx",    0xcc800000, 0xfc000000, PROPELLER_OP_DS, 4, 4 },
  { "cmpx",    0xcc000000, 0xfc000000, PROPELLER_OP_DS, 4, 4 },
  { "adds",    0xd0800000, 0xfc000000, PROPELLER_OP_DS, 4, 4 },
  { "subs",    0xd4800000, 0xfc000000, PROPELLER_OP_DS, 4, 4 },
  { "addsx",   0xd8800000, 0xfc000000, PROPELLER_OP_DS, 4, 4 },
  { "subsx",   0xdc800000, 0xfc000000, PROPELLER_OP_DS, 4, 4 },
  { "cmpsub",  0xe0800000, 0xfc000000, PROPELLER_OP_DS, 4, 4 },
  { "djnz",    0xe4800000, 0xfc000000, PROPELLER_OP_DS, 4, -8 },
  { "tjnz",    0xe8000000, 0xfc000000, PROPELLER_OP_DS, 4, -8 },
  { "tjz",     0xec000000, 0xfc000000, PROPELLER_OP_DS, 4, -8 },
  { "waitpeq", 0xf0000000, 0xfc000000, PROPELLER_OP_DS, 6, 0 },
  { "waitpne", 0xf4000000, 0xfc000000, PROPELLER_OP_DS, 6, 0 },
  { "waitcnt", 0xf8800000, 0xfc000000, PROPELLER_OP_DS, 6, 0 },
  { "waitvid", 0xfc000000, 0xfc000000, PROPELLER_OP_DS, 4, 0 },
  { "nop",     0x00000000, 0x003c0000, PROPELLER_OP_NOP, 4, 4 },
};

