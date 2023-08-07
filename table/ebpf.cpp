/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2023 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "table/ebpf.h"

struct _table_ebpf table_ebpf[] =
{
  { "add",    0x07, OP_REG_IMM, SIZE_64   },
  { "add",    0x0f, OP_REG_REG, SIZE_64   },
  { "sub",    0x17, OP_REG_IMM, SIZE_64   },
  { "sub",    0x1f, OP_REG_REG, SIZE_64   },
  { "mul",    0x27, OP_REG_IMM, SIZE_64   },
  { "mul",    0x2f, OP_REG_REG, SIZE_64   },
  { "div",    0x37, OP_REG_IMM, SIZE_64   },
  { "div",    0x3f, OP_REG_REG, SIZE_64   },
  { "or",     0x47, OP_REG_IMM, SIZE_64   },
  { "or",     0x4f, OP_REG_REG, SIZE_64   },
  { "and",    0x57, OP_REG_IMM, SIZE_64   },
  { "and",    0x5f, OP_REG_REG, SIZE_64   },
  { "lsh",    0x67, OP_REG_IMM, SIZE_64   },
  { "lsh",    0x6f, OP_REG_REG, SIZE_64   },
  { "rsh",    0x77, OP_REG_IMM, SIZE_64   },
  { "rsh",    0x7f, OP_REG_REG, SIZE_64   },
  { "neg",    0x87, OP_REG,     SIZE_64   },
  { "mod",    0x97, OP_REG_IMM, SIZE_64   },
  { "mod",    0x9f, OP_REG_REG, SIZE_64   },
  { "xor",    0xa7, OP_REG_IMM, SIZE_64   },
  { "xor",    0xaf, OP_REG_REG, SIZE_64   },
  { "mov",    0xb7, OP_REG_IMM, SIZE_64   },
  { "mov",    0xbf, OP_REG_REG, SIZE_64   },
  { "arsh",   0xc7, OP_REG_IMM, SIZE_64   },
  { "arsh",   0xcf, OP_REG_REG, SIZE_64   },
  { "add32",  0x04, OP_REG_IMM, SIZE_32   },
  { "add32",  0x0c, OP_REG_REG, SIZE_32   },
  { "sub32",  0x14, OP_REG_IMM, SIZE_32   },
  { "sub32",  0x1c, OP_REG_REG, SIZE_32   },
  { "mul32",  0x24, OP_REG_IMM, SIZE_32   },
  { "mul32",  0x2c, OP_REG_REG, SIZE_32   },
  { "div32",  0x34, OP_REG_IMM, SIZE_32   },
  { "div32",  0x3c, OP_REG_REG, SIZE_32   },
  { "or32",   0x44, OP_REG_IMM, SIZE_32   },
  { "or32",   0x4c, OP_REG_REG, SIZE_32   },
  { "and32",  0x54, OP_REG_IMM, SIZE_32   },
  { "and32",  0x5c, OP_REG_REG, SIZE_32   },
  { "lsh32",  0x64, OP_REG_IMM, SIZE_32   },
  { "lsh32",  0x6c, OP_REG_REG, SIZE_32   },
  { "rsh32",  0x74, OP_REG_IMM, SIZE_32   },
  { "rsh32",  0x7c, OP_REG_REG, SIZE_32   },
  { "neg32",  0x84, OP_REG,     SIZE_32   },
  { "mod32",  0x94, OP_REG_IMM, SIZE_32   },
  { "mod32",  0x9c, OP_REG_REG, SIZE_32   },
  { "xor32",  0xa4, OP_REG_IMM, SIZE_32   },
  { "xor32",  0xac, OP_REG_REG, SIZE_32   },
  { "mov32",  0xb4, OP_REG_IMM, SIZE_32   },
  { "mov32",  0xbc, OP_REG_REG, SIZE_32   },
  { "arsh32", 0xc4, OP_REG_IMM, SIZE_32   },
  { "arsh32", 0xcc, OP_REG_REG, SIZE_32   },

  { "call",   0x85, OP_IMM,     SIZE_NONE },
  { "exit",   0x95, OP_NONE,    SIZE_NONE },
  { NULL,     0x0c, OP_NONE,    0         },
};

