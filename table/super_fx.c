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

#include <stdio.h>
#include <stdlib.h>
#include "table/super_fx.h"

struct _table_super_fx table_super_fx[] =
{
  { "stop",  0x00, 0, 0xff, OP_NONE,    0 },
  { "nop",   0x01, 0, 0xff, OP_NONE,    0 },
  { "cache", 0x02, 0, 0xff, OP_NONE,    0 },
  { "lsr",   0x03, 0, 0xff, OP_NONE,    0 },
  { "rol",   0x04, 0, 0xff, OP_NONE,    0 },
  { "bra",   0x05, 0, 0xff, OP_OFFSET,  0 },
  { "blt",   0x06, 0, 0xff, OP_OFFSET,  0 },
  { "bge",   0x07, 0, 0xff, OP_OFFSET,  0 },
  { "bne",   0x08, 0, 0xff, OP_OFFSET,  0 },
  { "beq",   0x09, 0, 0xff, OP_OFFSET,  0 },
  { "bpl",   0x0a, 0, 0xff, OP_OFFSET,  0 },
  { "bmi",   0x0b, 0, 0xff, OP_OFFSET,  0 },
  { "bcc",   0x0c, 0, 0xff, OP_OFFSET,  0 },
  { "bcs",   0x0d, 0, 0xff, OP_OFFSET,  0 },
  { "bvc",   0x0e, 0, 0xff, OP_OFFSET,  0 },
  { "bvs",   0x0f, 0, 0xff, OP_OFFSET,  0 },
  { "to",    0x10, 0, 0xf0, OP_REG,     0xffff },
  { "with",  0x20, 0, 0xf0, OP_REG,     0xffff },
  { "stw",   0x30, 0, 0xf0, OP_ATREG,   0x0fff },
  { "loop",  0x3c, 0, 0xff, OP_NONE,    0 },
  { "alt1",  0x3d, 0, 0xff, OP_NONE,    0 },
  { "alt2",  0x3e, 0, 0xff, OP_NONE,    0 },
  { "alt3",  0x3f, 0, 0xff, OP_NONE,    0 },
  { "ldw",   0x40, 0, 0xf0, OP_ATREG,   0x0fff },
  { "plot",  0x4c, 0, 0xff, OP_NONE,    0 },
  { "swap",  0x4d, 0, 0xff, OP_NONE,    0 },
  { "color", 0x4e, 0, 0xff, OP_NONE,    0 },
  { "not",   0x4f, 0, 0xff, OP_NONE,    0 },
  { "add",   0x50, 0, 0xf0, OP_REG,     0xffff },
  { "sub",   0x60, 0, 0xf0, OP_REG,     0xffff },
  { "merge", 0x70, 0, 0xff, OP_NONE,    0 },
  { "and",   0x70, 0, 0xf0, OP_REG,     0xfffe },
  { "mult",  0x80, 0, 0xf0, OP_REG,     0xffff },
  { "sbk",   0x90, 0, 0xff, OP_NONE,    0 },
  { "link",  0x90, 0, 0xf0, OP_N,       0x0104 },
  { "sex",   0x95, 0, 0xff, OP_NONE,    0 },
  { "asr",   0x96, 0, 0xff, OP_NONE,    0 },
  { "ror",   0x97, 0, 0xff, OP_NONE,    0 },
  //{ "jmp",   0x90, 0, 0xf0, OP_REG,     0x3f1c },
  { "jmp",   0x90, 0, 0xf0, OP_REG,     0x3f00 },  // <-- docs say r8-r13
  { "lob",   0x9e, 0, 0xff, OP_NONE,    0 },
  { "fmult", 0x9f, 0, 0xff, OP_NONE,    0 },
  { "ibt",   0xa0, 0, 0xf0, OP_REG_PP,  0xffff },
  { "from",  0xb0, 0, 0xf0, OP_REG,     0xffff },
  { "hib",   0xc0, 0, 0xff, OP_NONE,    0 },
  { "or",    0xc0, 0, 0xf0, OP_REG,     0xfffe },
  { "inc",   0xd0, 0, 0xf0, OP_REG,     0x7fff },
  { "getc",  0xdf, 0, 0xff, OP_NONE,    0 },
  { "dec",   0xe0, 0, 0xf0, OP_REG,     0x7fff },
  { "getb",  0xef, 0, 0xff, OP_NONE,    0 },
  { "iwt",   0xf0, 0, 0xf0, OP_REG_XX,  0xffff },
  { "stb",   0x30, 1, 0xf0, OP_ATREG,   0x0fff },
  //{ "ldb",   0x40, 1, 0xf0, OP_ATREG,   0xafff },
  { "ldb",   0x40, 1, 0xf0, OP_ATREG,   0x0fff },  // <-- docs say r0-r11
  { "rpix",  0x4c, 1, 0xff, OP_NONE,    0 },
  { "cmode", 0x4e, 1, 0xff, OP_NONE,    0 },
  { "adc",   0x50, 1, 0xf0, OP_REG,     0xffff },
  { "sbc",   0x60, 1, 0xf0, OP_REG,     0xffff },
  { "bic",   0x70, 1, 0xf0, OP_REG,     0xfffe },
  { "umult", 0x80, 1, 0xf0, OP_REG,     0xffff },
  { "div2",  0x96, 1, 0xff, OP_NONE,    0 },
  { "ljmp",  0x90, 1, 0xff, OP_REG,     0x3f00 },
  { "lmult", 0x9f, 1, 0xff, OP_NONE,    0 },
  { "lms",   0xa0, 1, 0xf0, OP_REG_ATYY,0xffff },
  { "xor",   0xc0, 1, 0xf0, OP_REG,     0xffff },
  { "getbh", 0xef, 1, 0xff, OP_NONE,    0 },
  { "lm",    0xf0, 1, 0xf0, OP_REG_ATXX,0xffff },
  { "add",   0x50, 2, 0xf0, OP_N,       0 },
  { "sub",   0x60, 2, 0xf0, OP_N,       0 },
  { "and",   0x70, 2, 0xf0, OP_N,       0x010f },
  { "mult",  0x80, 2, 0xf0, OP_N,       0 },
  { "sms",   0xa0, 2, 0xf0, OP_ATYY_REG,0xffff },
  { "or",    0xc0, 2, 0xf0, OP_N,       0x010f },
  { "ramb",  0xdf, 2, 0xff, OP_NONE,    0 },
  { "getbl", 0xef, 2, 0xff, OP_NONE,    0 },
  { "sm",    0xf0, 2, 0xf0, OP_ATXX_REG,0xffff },
  { "adc",   0x50, 3, 0xf0, OP_N,       0 },
  { "cmp",   0x60, 3, 0xf0, OP_REG,     0xffff },
  { "bic",   0x70, 3, 0xf0, OP_N,       0x010f },
  { "umult", 0x80, 3, 0xf0, OP_N,       0 },
  { "xor",   0xc0, 3, 0xf0, OP_N,       0 },
  { "romb",  0xdf, 3, 0xff, OP_NONE,    0 },
  { "getbs", 0xef, 3, 0xff, OP_NONE,    0 },
};

