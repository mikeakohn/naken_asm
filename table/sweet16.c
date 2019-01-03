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

#include "table/sweet16.h"

struct _table_sweet16 table_sweet16[] =
{
  { "set",  0x10, 0xf0, SWEET16_OP_REG_VALUE },
  { "ld",   0x20, 0xf0, SWEET16_OP_REG },
  { "st",   0x30, 0xf0, SWEET16_OP_REG },
  { "ld",   0x40, 0xf0, SWEET16_OP_AT_REG },
  { "st",   0x50, 0xf0, SWEET16_OP_AT_REG },
  { "ldd",  0x60, 0xf0, SWEET16_OP_AT_REG },
  { "std",  0x70, 0xf0, SWEET16_OP_AT_REG },
  { "pop",  0x80, 0xf0, SWEET16_OP_AT_REG },
  { "stp",  0x90, 0xf0, SWEET16_OP_AT_REG },
  { "add",  0xa0, 0xf0, SWEET16_OP_REG },
  { "sub",  0xb0, 0xf0, SWEET16_OP_REG },
  { "popd", 0xc0, 0xf0, SWEET16_OP_AT_REG },
  { "cpr",  0xd0, 0xf0, SWEET16_OP_REG },
  { "inr",  0xe0, 0xf0, SWEET16_OP_REG },
  { "dcr",  0xf0, 0xf0, SWEET16_OP_REG },
  { "rtn",  0x00, 0xff, SWEET16_OP_NONE },
  { "br",   0x01, 0xff, SWEET16_OP_EA },
  { "bnc",  0x02, 0xff, SWEET16_OP_EA },
  { "bc",   0x03, 0xff, SWEET16_OP_EA },
  { "bp",   0x04, 0xff, SWEET16_OP_EA },
  { "bm",   0x05, 0xff, SWEET16_OP_EA },
  { "bz",   0x06, 0xff, SWEET16_OP_EA },
  { "bnz",  0x07, 0xff, SWEET16_OP_EA },
  { "bm1",  0x08, 0xff, SWEET16_OP_EA },
  { "bnm1", 0x09, 0xff, SWEET16_OP_EA },
  { "bk",   0x0a, 0xff, SWEET16_OP_NONE },
  { "rs",   0x0b, 0xff, SWEET16_OP_NONE },
  { "bs",   0x0c, 0xff, SWEET16_OP_EA },
  { NULL,   0x00, 0xff, SWEET16_OP_ILLEGAL },
};

