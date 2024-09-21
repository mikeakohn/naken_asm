/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2024 by Michael Kohn
 *
 */

// Apollo Guidance Computer.

#include <stdlib.h>

#include "table/agc.h"

struct _table_agc table_agc[] =
{
  { "ad",     060000, 070000, AGC_OP_K12,  false, 2 },
  { "ads",    026000, 076000, AGC_OP_K10,  false, 2 },
  { "aug",    024000, 076000, AGC_OP_K10,  true,  2 },
  { "bzf",    010000, 070000, AGC_OP_K12,  true,  1 },
  { "bzmf",   060000, 070000, AGC_OP_K12,  true,  1 },
  { "ca",     030000, 070000, AGC_OP_K12,  false, 2 },
  { "cae",    030000, 070000, AGC_OP_K12,  false, 2 }, // same as ca
  { "caf",    030000, 070000, AGC_OP_K12,  false, 2 }, // same as ca
  { "ccs",    010000, 070000, AGC_OP_K10,  false, 2 },
  { "com",    040000, 077777, AGC_OP_NONE, false, 2 }, // same as cs A
  { "cs",     040000, 070000, AGC_OP_K12,  false, 2 },
  { "das",    020001, 076001, AGC_OP_K10,  false, 3 }, // huh?
  { "dca",    030001, 070001, AGC_OP_K12,  true,  3 },
  { "dcom",   040001, 077777, AGC_OP_NONE, true,  3 }, // same as dcs A
  { "dcs",    040001, 070001, AGC_OP_K12,  true,  3 },
  { "ddoubl", 020001, 077777, AGC_OP_NONE, true,  3 },
  { "dim",    026000, 076000, AGC_OP_K10,  false, 2 },
  { "double", 060000, 077777, AGC_OP_NONE, false, 2 }, // same as ad A
  { "dtcb",   052006, 077777, AGC_OP_NONE, false, 3 },
  { "dtcf",   052005, 077777, AGC_OP_NONE, false, 3 },
  { "dv",     010000, 076000, AGC_OP_K10,  true,  6 }, // bzf?
  { "dxch",   052001, 076001, AGC_OP_K10,  false, 3 },
  { "edrupt", 007000, 077000, AGC_OP_IO,   false, 3 },
  { "extend", 000006, 077777, AGC_OP_NONE, false, 1 }, // not used
  { "incr",   024000, 076001, AGC_OP_K10,  false, 2 },
  { "index",  050000, 076000, AGC_OP_K10,  false, 2 },
  { "ndx",    050000, 076000, AGC_OP_K10,  false, 2 }, // same as index
  { "inhint", 000004, 077777, AGC_OP_NONE, false, 2 }, // same as index
  { "lxch",   022000, 076000, AGC_OP_K10,  false, 2 },
  { "mask",   070000, 070000, AGC_OP_K12,  false, 2 },
  { "msk",    070000, 070000, AGC_OP_K12,  false, 2 }, // same as mask
  { "mp",     070000, 070000, AGC_OP_K12,  true,  2 },
  { "msu",    020000, 076000, AGC_OP_K10,  true,  2 },
  //{ "noop",   010000, 070000, AGC_OP_NONE, false, 2 }, // check this
  { "noop",   030000, 077777, AGC_OP_NONE, false, 2 }, // check this
  { "ovsk",   054000, 077777, AGC_OP_NONE, false, 2 },
  { "qxch",   022000, 076000, AGC_OP_K10,  true,  2 },
  { "rand",   002000, 077000, AGC_OP_IO,   true,  2 },
  { "read",   000000, 077000, AGC_OP_IO,   true,  2 },
  { "relint", 000003, 077777, AGC_OP_NONE, false, 2 },
  { "resume", 050017, 077777, AGC_OP_NONE, false, 2 },
  { "return", 000002, 077777, AGC_OP_NONE, false, 2 },
  { "ror",    004000, 077000, AGC_OP_IO,   true,  2 },
  { "rxor",   006000, 077000, AGC_OP_IO,   true,  2 },
  { "square", 070000, 077777, AGC_OP_NONE, true,  2 }, // same as mp A
  { "su",     060000, 076000, AGC_OP_K10,  true,  2 },
  { "tc",     000000, 070000, AGC_OP_K12,  false, 1 },
  { "tcr",    000000, 070000, AGC_OP_K12,  false, 1 }, // same as tc
  { "tcaa",   054005, 077777, AGC_OP_NONE, false, 2 },
  { "tcf",    010000, 070000, AGC_OP_K12,  false, 1 },
  { "ts",     054000, 076000, AGC_OP_K10,  false, 2 },
  { "wand",   003000, 077000, AGC_OP_IO,   true,  2 },
  { "wor",    005000, 077000, AGC_OP_IO,   true,  2 },
  { "write",  001000, 077000, AGC_OP_IO,   true,  2 },
  { "xch",    056000, 076000, AGC_OP_K10,  false, 2 },
  { "xlq",    000001, 077777, AGC_OP_NONE, false, 1 },
  { "xxalq",  000000, 077777, AGC_OP_NONE, false, 1 }, // alias of tc A
  { "zl",     022007, 077777, AGC_OP_NONE, false, 2 },
  { "zq",     022007, 077777, AGC_OP_NONE, true,  2 },
  { NULL,     000000, 000000, 0,           false, 0 }
};

