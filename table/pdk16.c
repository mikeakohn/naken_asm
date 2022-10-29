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
#include "table/pdk16.h"

struct _table_pdk16 table_pdk16[] =
{
  // Single name.
  { "nop",     0x0000, 0xffff, OP_NONE, 1, 1 },
  // Misc.
  { "addc",    0x0010, 0xffff, OP_A,    1, 1 },
  { "subc",    0x0011, 0xffff, OP_A,    1, 1 },
  { "izsn",    0x0012, 0xffff, OP_A,    1, 2 },
  { "dzsn",    0x0013, 0xffff, OP_A,    1, 2 },
  { "pcadd",   0x0017, 0xffff, OP_A,    1, 1 },
  { "not",     0x0018, 0xffff, OP_A,    1, 1 },
  { "neg",     0x0019, 0xffff, OP_A,    1, 1 },
  { "sr",      0x001a, 0xffff, OP_A,    1, 1 },
  { "sl",      0x001b, 0xffff, OP_A,    1, 1 },
  { "src",     0x001c, 0xffff, OP_A,    1, 1 },
  { "slc",     0x001d, 0xffff, OP_A,    1, 1 },
  { "swap",    0x001e, 0xffff, OP_A,    1, 1 },
  { "delay",   0x001f, 0xffff, OP_A,    1, 1 },
  { "wdreset", 0x0030, 0xffff, OP_NONE, 1, 1 },
  { "pushaf",  0x0032, 0xffff, OP_NONE, 1, 1 },
  { "popaf",   0x0033, 0xffff, OP_NONE, 1, 1 },
  { "reset",   0x0035, 0xffff, OP_NONE, 1, 1 },
  { "stopsys", 0x0036, 0xffff, OP_NONE, 1, 1 },
  { "stopexe", 0x0037, 0xffff, OP_NONE, 1, 1 },
  { "engint",  0x0038, 0xffff, OP_NONE, 1, 1 },
  { "disgint", 0x0039, 0xffff, OP_NONE, 1, 1 },
  { "ret",     0x003a, 0xffff, OP_NONE, 1, 1 },
  { "reti",    0x003b, 0xffff, OP_NONE, 1, 1 },
  { "mul",     0x003c, 0xffff, OP_NONE, 1, 1 },
  // Misc.
  { "pmode",   0x0040, 0xffe0, OP_K5,   1, 1 },
  { "popwpc",  0x0060, 0xfff0, OP_N4,   1, 1 },
  { "pushwpc", 0x0070, 0xfff0, OP_N4,   1, 1 },
  // 16 bit memory operations.
  { "stt16",   0x0200, 0xfe01, OP_M8,   1, 1 },
  { "ldt16",   0x0201, 0xfe01, OP_M8,   1, 1 },
  { "popw",    0x0400, 0xfe01, OP_M8,   1, 1 },
  { "pushw",   0x0401, 0xfe01, OP_M8,   1, 1 },
  { "igoto",   0x0600, 0xfe01, OP_M8,   1, 1 },
  { "icall",   0x0601, 0xfe01, OP_M8,   1, 1 },
  { "idxm",    0x0800, 0xfe01, OP_M8_A, 2, 2 },
  { "idxm",    0x0801, 0xfe01, OP_A_M8, 2, 2 },
  { "ldtabl",  0x0a00, 0xfe01, OP_M8,   1, 1 },
  { "ldtabh",  0x0a00, 0xfe01, OP_M8,   1, 1 },
  // Operations with 8 bit literal.
  { "delay",   0x0e00, 0xff00, OP_K8,   1, 1 },
  { "ret",     0x0f00, 0xff00, OP_K8,   1, 1 },
  // Operations with A and IO.
  { "xor.io",  0x1000, 0xffc0, OP_IO_A, 1, 1 },
  { "xor.io",  0x1040, 0xffc0, OP_A_IO, 1, 1 },
  // Operations with A and memory.
  { "cneqsn",  0x1400, 0xfe00, OP_A_M9, 1, 1 },
  { "cneqsn",  0x1600, 0xfe00, OP_A_M9, 1, 1 },
  // Operations with A and 8 bit literal.
  { "add",     0x1800, 0xff00, OP_A_K,  1, 1 },
  { "sub",     0x1900, 0xff00, OP_A_K,  1, 1 },
  { "ceqsn",   0x1a00, 0xff00, OP_A_K,  1, 2 },
  { "cneqsn",  0x1b00, 0xff00, OP_A_K,  1, 2 },
  { "and",     0x1c00, 0xff00, OP_A_K,  1, 1 },
  { "or",      0x1d00, 0xff00, OP_A_K,  1, 1 },
  { "xor",     0x1e00, 0xff00, OP_A_K,  1, 1 },
  { "mov",     0x1f00, 0xff00, OP_A_K,  1, 1 },
  // Bit operations with IO.
  { "t0sn.io", 0x2000, 0xfe00, OP_IO_N, 1, 2 },
  { "t1sn.io", 0x2200, 0xfe00, OP_IO_N, 1, 2 },
  { "set0.io", 0x2400, 0xfe00, OP_IO_N, 1, 1 },
  { "set1.io", 0x2600, 0xfe00, OP_IO_N, 1, 1 },
  { "tog.io",  0x2800, 0xfe00, OP_IO_N, 1, 1 },
  { "wait0.io",0x2a00, 0xfe00, OP_IO_N, 1, 1 },
  { "wait1.io",0x2c00, 0xfe00, OP_IO_N, 1, 1 },
  { "swapc.io",0x2e00, 0xfe00, OP_IO_N, 1, 1 },
  // Operations with A and memory.
  { "nmov",    0x3000, 0xfe00, OP_A_M9, 1, 1 },
  { "nmov",    0x3200, 0xfe00, OP_M9_A, 1, 1 },
  { "nadd",    0x3400, 0xfe00, OP_A_M9, 1, 1 },
  { "nadd",    0x3600, 0xfe00, OP_M9_A, 1, 1 },
  { "ceqsn",   0x3800, 0xfe00, OP_A_M9, 1, 2 },
  { "ceqsn",   0x3a00, 0xfe00, OP_M9_A, 1, 2 },
  { "comp",    0x3c00, 0xfe00, OP_A_M9, 1, 1 },
  { "comp",    0x3e00, 0xfe00, OP_M9_A, 1, 1 },
  { "add",     0x4000, 0xff00, OP_M9_A, 1, 1 },
  { "add",     0x4200, 0xfe00, OP_A_M9, 1, 1 },
  { "sub",     0x4400, 0xfe00, OP_M9_A, 1, 1 },
  { "sub",     0x4600, 0xfe00, OP_A_M9, 1, 1 },
  { "addc",    0x4800, 0xfe00, OP_M9_A, 1, 1 },
  { "addc",    0x4a00, 0xff00, OP_A_M9, 1, 1 },
  { "subc",    0x4c00, 0xfe00, OP_M9_A, 1, 1 },
  { "subc",    0x4e00, 0xff00, OP_A_M9, 1, 1 },
  { "and",     0x5000, 0xfe00, OP_M9_A, 1, 1 },
  { "and",     0x5200, 0xff00, OP_A_M9, 1, 1 },
  { "or",      0x5400, 0xfe00, OP_M9_A, 1, 1 },
  { "or",      0x5600, 0xfe00, OP_A_M9, 1, 1 },
  { "xor",     0x5800, 0xfe00, OP_M9_A, 1, 1 },
  { "xor",     0x5a00, 0xfe00, OP_A_M9, 1, 1 },
  { "mov",     0x5c00, 0xfe00, OP_M9_A, 1, 1 },
  { "mov",     0x5e00, 0xfe00, OP_A_M9, 1, 1 },
  // Operations with memory.
  { "addc",    0x6000, 0xfe00, OP_M9,   1, 1 },
  { "subc",    0x6200, 0xfe00, OP_M9,   1, 1 },
  { "izsn",    0x6400, 0xfe00, OP_M9,   1, 2 },
  { "dzsn",    0x6600, 0xfe00, OP_M9,   1, 2 },
  { "inc",     0x6800, 0xfe00, OP_M9,   1, 1 },
  { "dec",     0x6a00, 0xfe00, OP_M9,   1, 1 },
  { "clear",   0x6c00, 0xfe00, OP_M9,   1, 1 },
  { "xch",     0x6e00, 0xfe00, OP_M9,   1, 1 },
  { "not",     0x7000, 0xfe00, OP_M9,   1, 1 },
  { "neg",     0x7200, 0xfe00, OP_M9,   1, 1 },
  { "sr",      0x7400, 0xfe00, OP_M9,   1, 1 },
  { "sl",      0x7600, 0xfe00, OP_M9,   1, 1 },
  { "src",     0x7800, 0xfe00, OP_M9,   1, 1 },
  { "slc",     0x7a00, 0xfe00, OP_M9,   1, 1 },
  { "swap",    0x7c00, 0xfe00, OP_M9,   1, 1 },
  { "delay",   0x7e00, 0xfe00, OP_M9,   1, 1 },
  // Bit operations with memory.
  { "t0sn",    0x8000, 0xf000, OP_M_N,  1, 2 },
  { "t1sn",    0x9400, 0xf000, OP_M_N,  1, 2 },
  { "set0",    0xa800, 0xf000, OP_M_N,  1, 1 },
  { "set1",    0xbc00, 0xf000, OP_M_N,  1, 1 },
  // Control transfers.
  { "goto",    0xc000, 0xf000, OP_K13,  2, 2 },
  { "call",    0xe000, 0xf000, OP_K13,  2, 2 },
  { NULL,      0x0000, 0x0000,      0,  0, 0 }
};

