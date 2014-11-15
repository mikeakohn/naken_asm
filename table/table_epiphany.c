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
  { "add", 0x000a001f, 0x000f007f, OP_REG_3_32, 32, 0, 0 },
  { "sub", 0x000a003f, 0x000f007f, OP_REG_3_32, 32, 0, 0 },
  { "and", 0x000a005f, 0x000f007f, OP_REG_3_32, 32, 0, 0 },
  { "orr", 0x000a007f, 0x000f007f, OP_REG_3_32, 32, 0, 0 },
  { "eor", 0x000a000f, 0x000f007f, OP_REG_3_32, 32, 0, 0 },
  { "asr", 0x000a006f, 0x000f007f, OP_REG_3_32, 32, 0, 0 },
  { "lsr", 0x000a004f, 0x000f007f, OP_REG_3_32, 32, 0, 0 },
  { "lsl", 0x000a002f, 0x000f007f, OP_REG_3_32, 32, 0, 0 },

  { "fadd",  0x0007, 0x007f, OP_REG_3_16, 16, 0, 0 },
  { "iadd",  0x0007, 0x007f, OP_REG_3_16, 16, 0, 0 }, // DUP
  { "fsub",  0x0017, 0x007f, OP_REG_3_16, 16, 0, 0 },
  { "isub",  0x0017, 0x007f, OP_REG_3_16, 16, 0, 0 }, // DUP
  { "fmul",  0x0027, 0x007f, OP_REG_3_16, 16, 0, 0 },
  { "imul",  0x0027, 0x007f, OP_REG_3_16, 16, 0, 0 }, // DUP
  { "fmadd", 0x003a, 0x007f, OP_REG_3_16, 16, 0, 0 },
  { "imadd", 0x003a, 0x007f, OP_REG_3_16, 16, 0, 0 }, // DUP
  { "fmsub", 0x0047, 0x007f, OP_REG_3_16, 16, 0, 0 },
  { "imsub", 0x0047, 0x007f, OP_REG_3_16, 16, 0, 0 }, // DUP
  { "float", 0x0057, 0x03ff, OP_REG_2_16, 16, 0, 0 },
  { "fix",   0x0067, 0x03ff, OP_REG_2_16, 16, 0, 0 },
  { "fabs",  0x0077, 0x03ff, OP_REG_2_16, 16, 0, 0 },
  { "fadd",  0x0007000f, 0x000f007f, OP_REG_3_32, 32, 0, 0 },
  { "iadd",  0x0007000f, 0x000f007f, OP_REG_3_32, 32, 0, 0 }, // DUP
  { "fsub",  0x0007001f, 0x000f007f, OP_REG_3_32, 32, 0, 0 },
  { "isub",  0x0007001f, 0x000f007f, OP_REG_3_32, 32, 0, 0 }, // DUP
  { "fmul",  0x0007002f, 0x000f007f, OP_REG_3_32, 32, 0, 0 },
  { "imul",  0x0007002f, 0x000f007f, OP_REG_3_32, 32, 0, 0 }, // DUP
  { "fmadd", 0x0007003f, 0x000f007f, OP_REG_3_32, 32, 0, 0 },
  { "imadd", 0x0007003f, 0x000f007f, OP_REG_3_32, 32, 0, 0 }, // DUP
  { "fmsub", 0x0007004f, 0x000f007f, OP_REG_3_32, 32, 0, 0 },
  { "imsub", 0x0007004f, 0x000f007f, OP_REG_3_32, 32, 0, 0 }, // DUP
  { "float", 0x0007005f, 0x000f03ff, OP_REG_2_32, 32, 0, 0 },
  { "fix",   0x0007006f, 0x000f03ff, OP_REG_2_32, 32, 0, 0 },
  { "fabs",  0x0007007f, 0x000f03ff, OP_REG_2_32, 32, 0, 0 },

  { "gie",    0x0192, 0xffff, OP_NONE_16, 16, 0, 0 },
  { "gid",    0x0392, 0xffff, OP_NONE_16, 16, 0, 0 },
  { "nop",    0x01a2, 0xffff, OP_NONE_16, 16, 0, 0 },
  { "idle",   0x01b2, 0xffff, OP_NONE_16, 16, 0, 0 },
  { "bkpt",   0x01c2, 0xffff, OP_NONE_16, 16, 0, 0 },
  { "mbkpt",  0x03c2, 0xffff, OP_NONE_16, 16, 0, 0 },
  { "sync",   0x01f2, 0xffff, OP_NONE_16, 16, 0, 0 },
  { "rti",    0x01d2, 0xffff, OP_NONE_16, 16, 0, 0 },
  { "wand",   0x0182, 0xffff, OP_NONE_16, 16, 0, 0 },
  { "trap",   0x03e2, 0x03ff, OP_NUM_16, 16, 0, 0 },
  { "unimpl",  0x000f000f, 0xffffffff, OP_NONE_32, 16, 0, 0 },
  { NULL, 0 }
};


