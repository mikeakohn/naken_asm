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

#include <stdlib.h>

#include "table/f8.h"

struct _table_f8 table_f8[] = {
  // I/O opcodes.
  { "ins",  0xa0, 0xf0, F8_OP_DATA4,  16, 16 },
  { "in",   0x26, 0xff, F8_OP_DATA8,  16, 16 },
  { "outs", 0xb0, 0xf0, F8_OP_DATA4,  16, 16 },
  { "out",  0x27, 0xff, F8_OP_DATA8,  16, 16 },

  // Primary memory reference opcodes.
  { "lm",   0x16, 0xff, F8_OP_NONE,   10, 10 },
  { "st",   0x17, 0xff, F8_OP_NONE,   10, 10 },
  { "lr",   0x40, 0xf0, F8_OP_A_R,     4,  4 },
  { "lr",   0x00, 0xfc, F8_OP_A_DPCHR, 4,  4 },
  { "lr",   0x50, 0xf0, F8_OP_R_A,     4,  4 },
  { "lr",   0x04, 0xfc, F8_OP_DPCHR_A, 4,  4 },
  { "lr",   0x11, 0xff, F8_OP_H_DC0,  16, 16 },
  { "lr",   0x0e, 0xff, F8_OP_Q_DC0,  16, 16 },
  { "lr",   0x10, 0xff, F8_OP_DC0_H,  16, 16 },
  { "lr",   0x0f, 0xff, F8_OP_DC0_Q,  16, 16 },
  { "lr",   0x08, 0xff, F8_OP_K_PC1,  16, 16 },
  { "lr",   0x09, 0xff, F8_OP_PC1_K,  16, 16 },
  { "lr",   0x0d, 0xff, F8_OP_PC0_Q,  16, 16 },
  { "pk",   0x0c, 0xff, F8_OP_NONE,   16, 16 },

  // Secondary memory reference (scratchpad) opcodes.
  { "as",   0xc0, 0xf0, F8_OP_R,       4,  4 },
  { "asd",  0xd0, 0xf0, F8_OP_R,       8,  8 },
  { "ns",   0xf0, 0xf0, F8_OP_R,       4,  4 },
  { "xs",   0xe0, 0xf0, F8_OP_R,       4,  4 },
  { "ds",   0x30, 0xf0, F8_OP_R,       6,  6 },
  { "am",   0x88, 0xff, F8_OP_NONE,   10, 10 },
  { "amd",  0x89, 0xff, F8_OP_NONE,   10, 10 },
  { "nm",   0x8a, 0xff, F8_OP_NONE,   10, 10 },
  { "om",   0x8b, 0xff, F8_OP_NONE,   10, 10 },
  { "xm",   0x8c, 0xff, F8_OP_NONE,   10, 10 },
  { "cm",   0x8d, 0xff, F8_OP_NONE,   10, 10 },

  // Immediate opcodes.
  { "lisu", 0x60, 0xf8, F8_OP_DATA3,   4,  4 },
  { "lisl", 0x68, 0xf8, F8_OP_DATA3,   4,  4 },
  { "dci",  0x2a, 0xff, F8_OP_ADDR,   14, 14 },
  { "lis",  0x70, 0xf0, F8_OP_DATA4,   4,  4 },
  { "li",   0x20, 0xff, F8_OP_DATA8,  10, 10 },
  { "ai",   0x24, 0xff, F8_OP_DATA8,  10, 10 },
  { "ni",   0x21, 0xff, F8_OP_DATA8,  10, 10 },
  { "oi",   0x22, 0xff, F8_OP_DATA8,  10, 10 },
  { "xi",   0x23, 0xff, F8_OP_DATA8,  10, 10 },
  { "ci",   0x25, 0xff, F8_OP_DATA8,  10, 10 },

  // Jump opcodes.
  { "pi",   0x28, 0xff, F8_OP_ADDR,   26, 26 },
  { "br",   0x90, 0xff, F8_OP_DISP,   14, 14 },
  { "jmp",  0x29, 0xff, F8_OP_ADDR,   14, 14 },

  // Conditional branch.
  { "bp",   0x81, 0xff, F8_OP_DISP,   12, 14 },
  { "bc",   0x82, 0xff, F8_OP_DISP,   12, 14 },
  { "bz",   0x84, 0xff, F8_OP_DISP,   12, 14 },
  { "bm",   0x91, 0xff, F8_OP_DISP,   12, 14 },
  { "bnc",  0x92, 0xff, F8_OP_DISP,   12, 14 },
  { "bnz",  0x94, 0xff, F8_OP_DISP,   12, 14 },
  { "bno",  0x98, 0xff, F8_OP_DISP,   12, 14 },
  { "br7",  0x8f, 0xff, F8_OP_DISP,   12, 14 },
  { "bt",   0x80, 0xf8, F8_OP_DATA3_DISP, 12, 14 },
  { "bf",   0x90, 0xf8, F8_OP_DATA3_DISP, 12, 14 },

  // Register to register move.
  { "xdc",  0x2c, 0xff, F8_OP_NONE,    8,  8 },
  { "lr",   0x0a, 0xff, F8_OP_A_IS,    4,  4 },
  { "lr",   0x0b, 0xff, F8_OP_IS_A,    4,  4 },
  { "pop",  0x1c, 0xff, F8_OP_NONE,    8,  8 },

  // Register to register operate.
  { "adc",  0x8e, 0xff, F8_OP_NONE,   10, 10 },

  // Reigster operate.
  { "sr",   0x12, 0xff, F8_OP_SHIFT_1, 4,  4 },
  { "sr",   0x14, 0xff, F8_OP_SHIFT_4, 4,  4 },
  { "sl",   0x13, 0xff, F8_OP_SHIFT_1, 4,  4 },
  { "sl",   0x15, 0xff, F8_OP_SHIFT_4, 4,  4 },
  { "com",  0x18, 0xff, F8_OP_NONE,    4,  4 },
  { "lnk",  0x19, 0xff, F8_OP_NONE,    4,  4 },
  { "inc",  0x1f, 0xff, F8_OP_NONE,    4,  4 },
  { "clr",  0x70, 0xff, F8_OP_NONE,    4,  4 },

  // Interrupt opcodes.
  { "di",   0x1a, 0xff, F8_OP_NONE,    8,  8 },
  { "ei",   0x1b, 0xff, F8_OP_NONE,    8,  8 },

  // Status register.
  { "lr",   0x1d, 0xff, F8_OP_W_J,     8,  8 },
  { "lr",   0x1e, 0xff, F8_OP_J_W,     4,  4 },

  // Misc opcode.
  { "nop",  0x2b, 0xff, F8_OP_NONE,    4,  4 },

  { NULL,   0x00, 0xff, 0,             0,  0 },
};

