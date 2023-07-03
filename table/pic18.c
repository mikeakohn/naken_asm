/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2023 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include "table/pic18.h"

struct _table_pic18 table_pic18[] =
{
  { "addwf",   0x2400, 0xfc00, OP_F_D_A,     1,  1 },
  { "addwfc",  0x2000, 0xfc00, OP_F_D_A,     1,  1 },
  { "andwf",   0x1400, 0xfc00, OP_F_D_A,     1,  1 },
  { "clrf",    0x6a00, 0xfe00, OP_F_A,       1,  1 },
  { "comf",    0x1c00, 0xfc00, OP_F_D_A,     1,  1 },
  { "cpfseq",  0x6200, 0xfe00, OP_F_A,       1,  3 },
  { "cpfsgt",  0x6400, 0xfe00, OP_F_A,       1,  3 },
  { "cpfslt",  0x6000, 0xfe00, OP_F_A,       1,  3 },
  { "decf",    0x0400, 0xfc00, OP_F_D_A,     1,  1 },
  { "decfsz",  0x2c00, 0xfc00, OP_F_D_A,     1,  3 },
  { "decfsnz", 0x4c00, 0xfc00, OP_F_D_A,     1,  3 },
  { "incf",    0x2800, 0xfc00, OP_F_D_A,     1,  1 },
  { "incfsz",  0x3c00, 0xfc00, OP_F_D_A,     1,  3 },
  { "incfsnz", 0x4800, 0xfc00, OP_F_D_A,     1,  3 },
  { "iorwf",   0x1000, 0xfc00, OP_F_D_A,     1,  1 },
  { "movf",    0x5000, 0xfc00, OP_F_D_A,     1,  1 },
  { "movff",   0xc000, 0xf000, OP_FS_FD,     2,  2 },
  { "movwf",   0x6e00, 0xfe00, OP_F_A,       1,  1 },
  { "mulwf",   0x0200, 0xfe00, OP_F_A,       1,  1 },
  { "negf",    0x6c00, 0xfe00, OP_F_A,       1,  1 },
  { "rlcf",    0x3400, 0xfc00, OP_F_D_A,     1,  1 },
  { "rlncf",   0x4400, 0xfc00, OP_F_D_A,     1,  1 },
  { "rrcf",    0x3000, 0xfc00, OP_F_D_A,     1,  1 },
  { "rrncf",   0x4000, 0xfc00, OP_F_D_A,     1,  1 },
  { "setf",    0x6000, 0xfe00, OP_F_A,       1,  1 },
  { "subfwb",  0x5400, 0xfc00, OP_F_D_A,     1,  1 },
  { "subwf",   0x5c00, 0xfc00, OP_F_D_A,     1,  1 },
  { "subwfb",  0x5800, 0xfc00, OP_F_D_A,     1,  1 },
  { "swapf",   0x3800, 0xfc00, OP_F_D_A,     1,  1 },
  { "tstfsz",  0x6600, 0xfe00, OP_F_A,       1,  3 },
  { "xorwf",   0x1800, 0xfc00, OP_F_D_A,     1,  1 },
  { "bcf",     0x9000, 0xf000, OP_F_B_A,     1,  1 },
  { "bsf",     0x8000, 0xf000, OP_F_B_A,     1,  1 },
  { "btfsc",   0xb000, 0xf000, OP_F_B_A,     1,  3 },
  { "btfss",   0xa000, 0xf000, OP_F_B_A,     1,  3 },
  { "btg",     0x7000, 0xf000, OP_F_B_A,     1,  1 },
  { "bc",      0xe200, 0xff00, OP_BRANCH_8,  1,  2 },
  { "bn",      0xe600, 0xff00, OP_BRANCH_8,  1,  2 },
  { "bnc",     0xe300, 0xff00, OP_BRANCH_8,  1,  2 },
  { "bnn",     0xe700, 0xff00, OP_BRANCH_8,  1,  2 },
  { "bnov",    0xe500, 0xff00, OP_BRANCH_8,  1,  2 },
  { "bnz",     0xe100, 0xff00, OP_BRANCH_8,  1,  2 },
  { "bov",     0xe400, 0xff00, OP_BRANCH_8,  1,  2 },
  { "bra",     0xd000, 0xf800, OP_BRANCH_11, 2,  2 },
  { "bz",      0xe000, 0xff00, OP_BRANCH_8,  1,  2 },
  { "call",    0xec00, 0xfe00, OP_CALL,      2,  2 },
  { "clrwdt",  0x0004, 0xffff, OP_NONE,      1,  1 },
  { "daw",     0x0007, 0xffff, OP_NONE,      1,  1 },
  { "goto",    0xef00, 0xff00, OP_GOTO,      2,  2 },
  { "nop",     0x0000, 0xffff, OP_NONE,      1,  1 },
  { "nop",     0xf000, 0xf000, OP_NONE,      1,  1 },
  { "pop",     0x0006, 0xffff, OP_NONE,      1,  1 },
  { "push",    0x0005, 0xffff, OP_NONE,      1,  1 },
  { "rcall",   0xd800, 0xf800, OP_BRANCH_11, 1,  2 },
  { "reset",   0x00ff, 0xffff, OP_NONE,      1,  1 },
  { "retfie",  0x0010, 0xfffe, OP_S,         2,  2 },
  { "retlw",   0x0c00, 0xff00, OP_K8,        2,  2 },
  { "sleep",   0x0003, 0xffff, OP_NONE,      1,  1 },
  { "return",  0x0012, 0xfffe, OP_S,         1,  1 },
  { "addlw",   0x0f00, 0xff00, OP_K8,        1,  1 },
  { "andlw",   0x0b00, 0xff00, OP_K8,        1,  1 },
  { "lfsr",    0xee00, 0xffc0, OP_F_K12,     2,  2 },
  { "movlb",   0x0100, 0xfff0, OP_K4,        1,  1 },
  { "movlw",   0x0e00, 0xff00, OP_K8,        1,  1 },
  { "mullw",   0x0d00, 0xff00, OP_K8,        1,  1 },
  { "sublw",   0x0800, 0xff00, OP_K8,        1,  1 },
  { "xorlw",   0x0a00, 0xff00, OP_K8,        1,  1 },
  { "tblrd*",  0x0008, 0xffff, OP_NONE,      2,  2 },
  { "tblrd*+", 0x0009, 0xffff, OP_NONE,     -1, -1 },
  { "tblrd*-", 0x000a, 0xffff, OP_NONE,     -1, -1 },
  { "tblrd+*", 0x000b, 0xffff, OP_NONE,     -1, -1 },
  { "tblwt*",  0x000c, 0xffff, OP_NONE,      2,  2 },
  { "tblwt*+", 0x000d, 0xffff, OP_NONE,     -1, -1 },
  { "tblwt*-", 0x000e, 0xffff, OP_NONE,     -1, -1 },
  { "tblwt+*", 0x000f, 0xffff, OP_NONE,     -1, -1 },
  { NULL,      0x0000, 0x0000,       0,      0,  0 }
};

