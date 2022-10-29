/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2022 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "table/pdk14.h"

struct _table_pdk14 table_pdk14[] =
{
  // Single name.
  { "nop",     0x0000, 0xffff, OP_NONE, 1, 1 },
  { "ldsptl",  0x0006, 0xffff, OP_NONE, 1, 1 },
  { "ldspth",  0x0007, 0xffff, OP_NONE, 1, 1 },
  // Misc.
  { "addc",    0x0060, 0xffff, OP_A,    1, 1 },
  { "subc",    0x0061, 0xffff, OP_A,    1, 1 },
  { "izsn",    0x0062, 0xffff, OP_A,    1, 2 },
  { "dzsn",    0x0063, 0xffff, OP_A,    1, 2 },
  { "pcadd",   0x0067, 0xffff, OP_A,    1, 1 },
  { "not",     0x0068, 0xffff, OP_A,    1, 1 },
  { "neg",     0x0069, 0xffff, OP_A,    1, 1 },
  { "sr",      0x006a, 0xffff, OP_A,    1, 1 },
  { "sl",      0x006b, 0xffff, OP_A,    1, 1 },
  { "src",     0x006c, 0xffff, OP_A,    1, 1 },
  { "slc",     0x006d, 0xffff, OP_A,    1, 1 },
  { "swap",    0x006e, 0xffff, OP_A,    1, 1 },
  { "wdreset", 0x0070, 0xffff, OP_NONE, 1, 1 },
  { "pushaf",  0x0072, 0xffff, OP_NONE, 1, 1 },
  { "popaf",   0x0073, 0xffff, OP_NONE, 1, 1 },
  { "reset",   0x0075, 0xffff, OP_NONE, 1, 1 },
  { "stopsys", 0x0076, 0xffff, OP_NONE, 1, 1 },
  { "stopexe", 0x0077, 0xffff, OP_NONE, 1, 1 },
  { "engint",  0x0078, 0xffff, OP_NONE, 1, 1 },
  { "disgint", 0x0079, 0xffff, OP_NONE, 1, 1 },
  { "ret",     0x007a, 0xffff, OP_NONE, 1, 1 },
  { "reti",    0x007b, 0xffff, OP_NONE, 1, 1 },
  { "mul",     0x007c, 0xffff, OP_NONE, 1, 1 },
  // Operations with A and IO.
  { "xor.io",  0x00c0, 0xffc0, OP_IO_A, 1, 1 },
  { "mov.io",  0x0180, 0xffc0, OP_IO_A, 1, 1 },
  { "mov.io",  0x01c0, 0xffc0, OP_A_IO, 1, 1 },
  // Return with A.
  { "ret",     0x0200, 0xff00, OP_K8,   1, 1 },
  // 16 bit memory operations.
  { "stt16",   0x0300, 0xff81, OP_M6,   1, 1 },
  { "ldt16",   0x0301, 0xff81, OP_M6,   1, 1 },
  { "idxm",    0x0380, 0xff81, OP_M6_A, 2, 2 },
  { "idxm",    0x0381, 0xff81, OP_A_M6, 2, 2 },
  // Special operation with CF and IO.
  { "swapc.io",0x0400, 0xfe00, OP_IO_N, 1, 1 },
  // Operations with A and memory.
  { "comp",    0x0600, 0xff80, OP_A_M,  1, 1 },
  { "comp",    0x0680, 0xff80, OP_M_A,  1, 1 },
  { "nadd",    0x0700, 0xff80, OP_A_M,  1, 1 },
  { "nadd",    0x0780, 0xff80, OP_M_A,  1, 1 },
  { "add",     0x0800, 0xff80, OP_M_A,  1, 1 },
  { "sub",     0x0880, 0xff80, OP_M_A,  1, 1 },
  { "addc",    0x0900, 0xff80, OP_M_A,  1, 1 },
  { "subc",    0x0980, 0xff80, OP_M_A,  1, 1 },
  { "and",     0x0a00, 0xff80, OP_M_A,  1, 1 },
  { "or",      0x0a80, 0xff80, OP_M_A,  1, 1 },
  { "xor",     0x0b00, 0xff80, OP_M_A,  1, 1 },
  { "mov",     0x0b80, 0xff80, OP_M_A,  1, 1 },
  { "add",     0x0c00, 0xff80, OP_A_M,  1, 1 },
  { "sub",     0x0c80, 0xff80, OP_A_M,  1, 1 },
  { "addc",    0x0d00, 0xff80, OP_A_M,  1, 1 },
  { "subc",    0x0d80, 0xff80, OP_A_M,  1, 1 },
  { "and",     0x0e00, 0xff80, OP_A_M,  1, 1 },
  { "or",      0x0e80, 0xff80, OP_A_M,  1, 1 },
  { "xor",     0x0f00, 0xff80, OP_A_M,  1, 1 },
  { "mov",     0x0f80, 0xff80, OP_A_M,  1, 1 },
  // Operations with memory.
  { "addc",    0x1000, 0xff80, OP_M,    1, 1 },
  { "subc",    0x1080, 0xff80, OP_M,    1, 1 },
  { "izsn",    0x1100, 0xff80, OP_M,    1, 2 },
  { "dzsn",    0x1180, 0xff80, OP_M,    1, 2 },
  { "inc",     0x1200, 0xff80, OP_M,    1, 1 },
  { "dec",     0x1280, 0xff80, OP_M,    1, 1 },
  { "clear",   0x1300, 0xff80, OP_M,    1, 1 },
  { "xch",     0x1380, 0xff80, OP_M,    1, 1 },
  { "not",     0x1400, 0xff80, OP_M,    1, 1 },
  { "neg",     0x1480, 0xff80, OP_M,    1, 1 },
  { "sr",      0x1500, 0xff80, OP_M,    1, 1 },
  { "sl",      0x1580, 0xff80, OP_M,    1, 1 },
  { "src",     0x1600, 0xff80, OP_M,    1, 1 },
  { "slc",     0x1680, 0xff80, OP_M,    1, 1 },
  { "ceqsn",   0x1700, 0xff80, OP_A_M,  1, 2 },
  { "cneqsn",  0x1780, 0xff80, OP_A_M,  1, 2 },
  // Bit operations with IO.
  { "t0sn.io", 0x1800, 0xfe00, OP_IO_N, 1, 2 },
  { "t1sn.io", 0x1a00, 0xfe00, OP_IO_N, 1, 2 },
  { "set0.io", 0x1c00, 0xfe00, OP_IO_N, 1, 1 },
  { "set1.io", 0x1e00, 0xfe00, OP_IO_N, 1, 1 },
  // Bit operations with memory.
  { "t0sn",    0x2000, 0xfe00, OP_M_N,  1, 2 },
  { "t1sn",    0x2200, 0xfe00, OP_M_N,  1, 2 },
  { "set0",    0x2400, 0xfe00, OP_M_N,  1, 1 },
  { "set1",    0x2600, 0xfe00, OP_M_N,  1, 1 },
  // Operations with A and 8 bit literal.
  { "add",     0x2800, 0xff00, OP_A_K,  1, 1 },
  { "sub",     0x2900, 0xff00, OP_A_K,  1, 1 },
  { "ceqsn",   0x2a00, 0xff00, OP_A_K,  1, 2 },
  { "cneqsn",  0x2b00, 0xff00, OP_A_K,  1, 2 },
  { "and",     0x2c00, 0xff00, OP_A_K,  1, 1 },
  { "or",      0x2d00, 0xff00, OP_A_K,  1, 1 },
  { "xor",     0x2e00, 0xff00, OP_A_K,  1, 1 },
  { "mov",     0x2f00, 0xff00, OP_A_K,  1, 1 },
  // Control transfers.
  { "goto",    0x3000, 0xf800, OP_K11,  2, 2 },
  { "call",    0x3800, 0xf800, OP_K11,  2, 2 },
  { NULL,      0x0000, 0x0000,      0,  0, 0 }
};

