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
#include "table/pdk15.h"

struct _table_pdk15 table_pdk15[] =
{
  // Single name.
  { "nop",     0x0000, 0xffff, OP_NONE, 1, 1 },
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
  { "xor.io",  0x0080, 0xffc0, OP_IO_A, 1, 1 },
  { "mov.io",  0x0100, 0xffc0, OP_IO_A, 1, 1 },
  { "mov.io",  0x0180, 0xffc0, OP_A_IO, 1, 1 },
  // Return with A.
  { "ret",     0x0200, 0xff00, OP_K8,   1, 1 },
  // 16 bit memory operations.
  { "ldtabl",  0x0500, 0xff01, OP_M7,   1, 1 },
  { "ldtabh",  0x0501, 0xff01, OP_M7,   1, 1 },
  { "stt16",   0x0600, 0xff01, OP_M7,   1, 1 },
  { "ldt16",   0x0601, 0xff01, OP_M7,   1, 1 },
  { "idxm",    0x0700, 0xff01, OP_M7_A, 2, 2 },
  { "idxm",    0x0701, 0xff01, OP_A_M7, 2, 2 },
  // Operations with A and memory.
  { "nmov",    0x0800, 0xff00, OP_A_M8, 1, 1 },
  { "nmov",    0x0900, 0xff00, OP_M8_A, 1, 1 },
  { "swap",    0x0a00, 0xff00, OP_M8,   1, 1 },
  { "comp",    0x0c00, 0xff00, OP_A_M8, 1, 1 },
  { "comp",    0x0d00, 0xff00, OP_M8_A, 1, 1 },
  { "nadd",    0x0e00, 0xff00, OP_A_M8, 1, 1 },
  { "nadd",    0x0f00, 0xff00, OP_M8_A, 1, 1 },
  { "add",     0x1000, 0xff00, OP_M8_A, 1, 1 },
  { "sub",     0x1100, 0xff00, OP_M8_A, 1, 1 },
  { "addc",    0x1200, 0xff00, OP_M8_A, 1, 1 },
  { "subc",    0x1300, 0xff00, OP_M8_A, 1, 1 },
  { "and",     0x1400, 0xff00, OP_M8_A, 1, 1 },
  { "or",      0x1500, 0xff00, OP_M8_A, 1, 1 },
  { "xor",     0x1600, 0xff00, OP_M8_A, 1, 1 },
  { "mov",     0x1700, 0xff00, OP_M8_A, 1, 1 },
  { "add",     0x1800, 0xff00, OP_A_M8, 1, 1 },
  { "sub",     0x1900, 0xff00, OP_A_M8, 1, 1 },
  { "addc",    0x1a00, 0xff00, OP_A_M8, 1, 1 },
  { "subc",    0x1b00, 0xff00, OP_A_M8, 1, 1 },
  { "and",     0x1c00, 0xff00, OP_A_M8, 1, 1 },
  { "or",      0x1d00, 0xff00, OP_A_M8, 1, 1 },
  { "xor",     0x1e00, 0xff00, OP_A_M8, 1, 1 },
  { "mov",     0x1f00, 0xff00, OP_A_M8, 1, 1 },
  // Operations with memory.
  { "addc",    0x2000, 0xff00, OP_M8,   1, 1 },
  { "subc",    0x2100, 0xff00, OP_M8,   1, 1 },
  { "izsn",    0x2200, 0xff00, OP_M8,   1, 2 },
  { "dzsn",    0x2300, 0xff00, OP_M8,   1, 2 },
  { "inc",     0x2400, 0xff00, OP_M8,   1, 1 },
  { "dec",     0x2500, 0xff00, OP_M8,   1, 1 },
  { "clear",   0x2600, 0xff00, OP_M8,   1, 1 },
  { "xch",     0x2700, 0xff00, OP_M8,   1, 1 },
  { "not",     0x2800, 0xff00, OP_M8,   1, 1 },
  { "neg",     0x2900, 0xff00, OP_M8,   1, 1 },
  { "sr",      0x2a00, 0xff00, OP_M8,   1, 1 },
  { "sl",      0x2b00, 0xff00, OP_M8,   1, 1 },
  { "src",     0x2c00, 0xff00, OP_M8,   1, 1 },
  { "slc",     0x2d00, 0xff00, OP_M8,   1, 1 },
  { "ceqsn",   0x2e00, 0xff00, OP_A_M8, 1, 2 },
  { "cneqsn",  0x2f00, 0xff00, OP_A_M8, 1, 2 },
  // Bit operations with IO.
  { "t0sn.io", 0x3000, 0xfc00, OP_IO_N, 1, 2 },
  { "t1sn.io", 0x3400, 0xfc00, OP_IO_N, 1, 2 },
  { "set0.io", 0x3800, 0xfc00, OP_IO_N, 1, 1 },
  { "set1.io", 0x3c00, 0xfc00, OP_IO_N, 1, 1 },
  // Bit operations with memory.
  { "t0sn",    0x4000, 0xfc00, OP_M_N,  1, 2 },
  { "t1sn",    0x4400, 0xfc00, OP_M_N,  1, 2 },
  { "set0",    0x4800, 0xfc00, OP_M_N,  1, 1 },
  { "set1",    0x4c00, 0xfc00, OP_M_N,  1, 1 },
  // Operations with A and 8 bit literal.
  { "add",     0x5000, 0xff00, OP_A_K,  1, 1 },
  { "sub",     0x5100, 0xff00, OP_A_K,  1, 1 },
  { "ceqsn",   0x5200, 0xff00, OP_A_K,  1, 2 },
  { "cneqsn",  0x5300, 0xff00, OP_A_K,  1, 2 },
  { "and",     0x5400, 0xff00, OP_A_K,  1, 1 },
  { "or",      0x5500, 0xff00, OP_A_K,  1, 1 },
  { "xor",     0x5600, 0xff00, OP_A_K,  1, 1 },
  { "mov",     0x5700, 0xff00, OP_A_K,  1, 1 },
  // Special operation with CF and IO.
  { "swapc.io",0x5c00, 0xfc00, OP_IO_N, 1, 1 },
  // Control transfers.
  { "goto",    0x6000, 0xf000, OP_K12,  2, 2 },
  { "call",    0x7000, 0xf000, OP_K12,  2, 2 },
  { NULL,      0x0000, 0x0000,      0,  0, 0 }
};

