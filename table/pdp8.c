/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2020 by Michael Kohn
 *
 * PDP-8 by Lars Brinkhoff
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "table/pdp8.h"

struct _table_pdp8 table_pdp8[] =
{
  // Memory operations
  { "and", 00000, 07000, OP_M },
  { "tad", 01000, 07000, OP_M },
  { "isz", 02000, 07000, OP_M },
  { "dca", 03000, 07000, OP_M },
  { "jms", 04000, 07000, OP_M },
  { "jmp", 05000, 07000, OP_M },

  // I/O operations
  { "iot", 06000, 07000, OP_IOT },

  // Microcoded operations
  // Note that the order is important for correct disassembly.
  { "nop", 07000, 07777, OP_NONE },
  { "cla", 07200, 07777, OP_NONE },
  { "cll", 07100, 07777, OP_NONE },
  { "tca", 07041, 07777, OP_NONE },
  { "cma", 07040, 07777, OP_NONE },
  { "cml", 07020, 07777, OP_NONE },
  { "iac", 07001, 07777, OP_NONE },
  { "rtl", 07006, 07777, OP_NONE },
  { "rtr", 07012, 07777, OP_NONE },
  { "bsw", 07002, 07777, OP_NONE },
  { "ral", 07004, 07777, OP_NONE },
  { "rar", 07010, 07777, OP_NONE },

  { "hlt", 07402, 07777, OP_NONE },
  { "osr", 07404, 07777, OP_NONE },

  { "sma", 07500, 07777, OP_NONE },
  { "sza", 07440, 07777, OP_NONE },
  { "snl", 07420, 07777, OP_NONE },
  { "cla", 07600, 07777, OP_NONE },

  { "spa", 07510, 07777, OP_NONE },
  { "sna", 07450, 07777, OP_NONE },
  { "szl", 07430, 07777, OP_NONE },
  { "skp", 07410, 07777, OP_NONE },
  { "cla", 07610, 07777, OP_NONE },

  { "cam", 07621, 07777, OP_NONE },
  { "cla", 07601, 07777, OP_NONE },
  { "mqa", 07501, 07777, OP_NONE },
  { "sca", 07441, 07777, OP_NONE },
  { "mql", 07421, 07777, OP_NONE },
  { "lsr", 07417, 07777, OP_NONE },
  { "shl", 07413, 07777, OP_NONE },
  { "asr", 07415, 07777, OP_NONE },
  { "nmi", 07411, 07777, OP_NONE },
  { "dvi", 07407, 07777, OP_NONE },
  { "scl", 07403, 07777, OP_NONE },
  { "muy", 07405, 07777, OP_NONE },

  { "opr", 07000, 07000, OP_OPR },
};

