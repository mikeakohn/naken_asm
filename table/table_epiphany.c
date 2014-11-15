/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2014 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "disasm_epiphany.h"
#include "table_epiphany.h"

struct _table_epiphany table_epiphany[] = {
  { "beq", 0x0000, 0x0000, OP_BRANCH, 32, 0, 0 },
  { "bne", 0x0001, 0x0000, OP_BRANCH, 32, 0, 0 },
  { "bgtu", 0x0002, 0x0000, OP_BRANCH, 32, 0, 0 },
  { "bgteu", 0x0003, 0x0000, OP_BRANCH, 32, 0, 0 },
  { "blteu", 0x0004, 0x0000, OP_BRANCH, 32, 0, 0 },
  { "bltu", 0x0005, 0x0000, OP_BRANCH, 32, 0, 0 },
  { "bgt", 0x0006, 0x0000, OP_BRANCH, 32, 0, 0 },
  { "bgte", 0x0007, 0x0000, OP_BRANCH, 32, 0, 0 },
  { "blt", 0x0008, 0x0000, OP_BRANCH, 32, 0, 0 },
  { "blte", 0x0009, 0x0000, OP_BRANCH, 32, 0, 0 },
  { "bbeq", 0x000a, 0x0000, OP_BRANCH, 32, 0, 0 },
  { "bbne", 0x000b, 0x0000, OP_BRANCH, 32, 0, 0 },
  { "bblt", 0x000c, 0x0000, OP_BRANCH, 32, 0, 0 },
  { "bblte", 0x000d, 0x0000, OP_BRANCH, 32, 0, 0 },
  { "b", 0x000e, 0x0000, OP_BRANCH, 32, 0, 0 },
  { "bl", 0x000f, 0x0000, OP_BRANCH, 32, 0, 0 },

  { "add", 0x001a, 0x007f, OP_REG_3_16, 16, 0, 0 },
  { "sub", 0x003a, 0x007f, OP_REG_3_16, 16, 0, 0 },
  { "and", 0x005a, 0x007f, OP_REG_3_16, 16, 0, 0 },
  { "orr", 0x007a, 0x007f, OP_REG_3_16, 16, 0, 0 },
  { "eor", 0x000a, 0x007f, OP_REG_3_16, 16, 0, 0 },
  { "asr", 0x006a, 0x007f, OP_REG_3_16, 16, 0, 0 },
  { "lsr", 0x004a, 0x007f, OP_REG_3_16, 16, 0, 0 },
  { "lsl", 0x002a, 0x007f, OP_REG_3_16, 16, 0, 0 },
  { "add", 0x000a001a, 0x000f007f, OP_REG_3_32, 32, 0, 0 },
  { "sub", 0x000a003a, 0x000f007f, OP_REG_3_32, 32, 0, 0 },
  { "and", 0x000a005a, 0x000f007f, OP_REG_3_32, 32, 0, 0 },
  { "orr", 0x000a007a, 0x000f007f, OP_REG_3_32, 32, 0, 0 },
  { "eor", 0x000a000a, 0x000f007f, OP_REG_3_32, 32, 0, 0 },
  { "asr", 0x000a006a, 0x000f007f, OP_REG_3_32, 32, 0, 0 },
  { "lsr", 0x000a004a, 0x000f007f, OP_REG_3_32, 32, 0, 0 },
  { "lsl", 0x000a002a, 0x000f007f, OP_REG_3_32, 32, 0, 0 },
  { NULL, 0 }
};


