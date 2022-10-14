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
#include "table/pic14.h"

struct _table_pic14 table_pic14[] =
{
  // Byte-Oriented file register operations
  { "addwf",  0x0700, 0xff00, OP_F_D,  1, 1 },
  { "andwf",  0x0500, 0xff00, OP_F_D,  1, 1 },
  { "clrf",   0x0180, 0xff80, OP_F,    1, 1 },
  { "clrw",   0x0103, 0xff80, OP_NONE, 1, 1 },
  { "comf",   0x0900, 0xff00, OP_F_D,  1, 1 },
  { "decf",   0x0300, 0xff00, OP_F_D,  1, 1 },
  { "decfsz", 0x0b00, 0xff00, OP_F_D,  1, 2 },
  { "incf",   0x0a00, 0xff00, OP_F_D,  1, 1 },
  { "incfsz", 0x0f00, 0xff00, OP_F_D,  1, 2 },
  { "iorwf",  0x0400, 0xff00, OP_F_D,  1, 1 },
  { "movf",   0x0800, 0xff00, OP_F_D,  1, 1 },
  { "movwf",  0x0080, 0xff80, OP_F,    1, 1 },
  { "nop",    0x0000, 0xff9f, OP_NONE, 1, 1 },
  { "rlf",    0x0d00, 0xff00, OP_F_D,  1, 1 },
  { "rrf",    0x0c00, 0xff00, OP_F_D,  1, 1 },
  { "subwf",  0x0200, 0xff00, OP_F_D,  1, 1 },
  { "swapf",  0x0e00, 0xff00, OP_F_D,  1, 1 },
  { "xorwf",  0x0600, 0xff00, OP_F_D,  1, 1 },
  // Bit-Oriented file register operations
  { "bcf",    0x1000, 0xfc00, OP_F_B,  1, 1 },
  { "bsf",    0x1400, 0xfc00, OP_F_B,  1, 1 },
  { "btfsc",  0x1800, 0xfc00, OP_F_B,  1, 2 },
  { "btfss",  0x1c00, 0xfc00, OP_F_B,  1, 2 },
  // Literal and control operations
  { "addlw",  0x3e00, 0xfe00, OP_K8,   1, 1 },
  { "andlw",  0x3900, 0xff00, OP_K8,   1, 1 },
  { "call",   0x2000, 0xf800, OP_K11,  2, 2 },
  { "clrwdt", 0x0064, 0xffff, OP_NONE, 1, 1 },
  { "goto",   0x2800, 0xf800, OP_K11,  2, 2 },
  { "iorlw",  0x3800, 0xff00, OP_K8,   1, 1 },
  { "movlw",  0x3000, 0xff00, OP_K8,   1, 1 },
  { "retfie", 0x0009, 0xffff, OP_NONE, 2, 2 },
  { "retlw",  0x3400, 0xfc00, OP_K8,   2, 2 },
  { "return", 0x0008, 0xffff, OP_NONE, 2, 2 },
  { "sleep",  0x0063, 0xffff, OP_NONE, 2, 2 },
  { "sublw",  0x3c00, 0xfe00, OP_K8,   1, 1 },
  { "xorlw",  0x3a00, 0xff00, OP_K8,   1, 1 },
  { NULL,     0x0000, 0x0000,     0,   0, 0 }
};

