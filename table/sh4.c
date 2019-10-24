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
#include "table/sh4.h"

struct _table_sh4 table_sh4[] =
{
  { "add",     0x300c, 0xf00f, OP_REG_REG },
  { "add",     0x7000, 0xf000, OP_IMM_REG },
  { "addc",    0x300e, 0xf00f, OP_REG_REG },
  { "addv",    0x300f, 0xf00f, OP_REG_REG },
  { "and",     0x2009, 0xf00f, OP_REG_REG },
  { "and",     0xc900, 0xff00, OP_IMM_R0 },
  { "and.b",   0xcd00, 0xff00, OP_IMM_AT_R0_GBR },
  { "bf",      0x8b00, 0xff00, OP_BRANCH_S9 },
  { "bf/s",    0x8f00, 0xff00, OP_BRANCH_S9 },
  { "bra",     0xa000, 0xf000, OP_BRANCH_S13 },
  { "braf",    0x0023, 0xf0ff, OP_REG },
  { "brk",     0x003b, 0xffff, OP_NONE },
  { "bsr",     0xb000, 0xf000, OP_BRANCH_S13 },
  { "bsrf",    0x0003, 0xf0ff, OP_REG },
  { "bt",      0x8900, 0xff00, OP_BRANCH_S9 },
  { "bt/s",    0x8d00, 0xff00, OP_BRANCH_S9 },
  { "clrmac",  0x0028, 0xffff, OP_NONE },
  { "clrs",    0x0048, 0xffff, OP_NONE },
  { "clrt",    0x0008, 0xffff, OP_NONE },
  { "cmp/eq",  0x3000, 0xf00f, OP_REG_REG },
  { "cmp/eq",  0x8800, 0xff00, OP_IMM_R0 },
  { "cmp/ge",  0x3003, 0xf00f, OP_REG_REG },
  { "cmp/gt",  0x3007, 0xf00f, OP_REG_REG },
  { "cmp/hi",  0x3006, 0xf00f, OP_REG_REG },
  { "cmp/hs",  0x3002, 0xf00f, OP_REG_REG },
  { "cmp/pl",  0x4015, 0xf0ff, OP_REG },
  { "cmp/pz",  0x4011, 0xf0ff, OP_REG },
  { "cmp/str", 0x200c, 0xf00f, OP_REG_REG },
  { "div0s",   0x2007, 0xf00f, OP_REG_REG },
  { "div0u",   0x0019, 0xffff, OP_NONE },
  { "div1",    0x3004, 0xf00f, OP_REG_REG },
  { "dmuls.l", 0x300d, 0xf00f, OP_REG_REG },
  { "dmulu.l", 0x3005, 0xf00f, OP_REG_REG },
  { "dt",      0x4010, 0xf0ff, OP_REG },
  { "exts.b",  0x600e, 0xf00f, OP_REG_REG },
  { "exts.w",  0x600f, 0xf00f, OP_REG_REG },
  { "extu.b",  0x600c, 0xf00f, OP_REG_REG },
  { "extu.w",  0x600d, 0xf00f, OP_REG_REG },
  { "fabs",    0xf05d, 0xf1ff, OP_DREG },
  { "fabs",    0xf05d, 0xf0ff, OP_FREG },
  { "fadd",    0xf000, 0xf11f, OP_DREG_DREG },
  { "fadd",    0xf000, 0xf00f, OP_FREG_FREG },
  { "fcmp/eq", 0xf004, 0xf00f, OP_FREG_FREG },
  { "fcmp/gt", 0xf005, 0xf00f, OP_DREG_DREG },
  { "fcnvds" , 0xf0bd, 0xf1ff, OP_DREG_FPUL },
  { "fcnvsd" , 0xf0ad, 0xf1ff, OP_FPUL_DREG },
  { "fdiv",    0xf003, 0xf11f, OP_DREG_DREG },
  { "fdiv",    0xf003, 0xf00f, OP_FREG_FREG },
  { "fipr",    0xf0ed, 0xf0ff, OP_FVREG_FVREG },
  { "flds",    0xf01d, 0xf0ff, OP_FREG_FPUL },
  { "fldi0",   0xf08d, 0xf0ff, OP_FREG },
  { "fldi1",   0xf09d, 0xf0ff, OP_FREG },
  { "float",   0xf02d, 0xf1ff, OP_FPUL_DREG },
  { "float",   0xf02d, 0xf0ff, OP_FPUL_FREG },
  { "fmac",    0xf00e, 0xf00f, OP_FR0_FREG_FREG },
  { "fmov",    0xf00c, 0xf11f, OP_DREG_DREG },
  { "fmov",    0xf10c, 0xf11f, OP_DREG_XDREG },
  { "fmov",    0xf00a, 0xf01f, OP_DREG_AT_REG },
  { "fmov",    0xf00b, 0xf01f, OP_DREG_AT_MINUS_REG },
  { "fmov",    0xf007, 0xf01f, OP_DREG_AT_R0_REG },
  { "fmov.s",  0xf00c, 0xf00f, OP_FREG_FREG },
  { "fmov.s",  0xf00a, 0xf00f, OP_FREG_AT_REG },
  { "fmov.s",  0xf00b, 0xf00f, OP_FREG_AT_MINUS_REG },
  { "fmov.s",  0xf007, 0xf00f, OP_FREG_AT_R0_REG },
  { "fmov",    0xf01c, 0xf11f, OP_XDREG_DREG },
  { "fmov",    0xf11c, 0xf11f, OP_XDREG_XDREG },
  { "fmov",    0xf01a, 0xf01f, OP_XDREG_AT_REG },
  { "fmov",    0xf01b, 0xf01f, OP_XDREG_AT_MINUS_REG },
  { "fmov",    0xf017, 0xf01f, OP_XDREG_AT_R0_REG },
  { "fmov",    0xf008, 0xf10f, OP_AT_REG_DREG },
  { "fmov",    0xf009, 0xf10f, OP_AT_REG_PLUS_DREG },
  { "fmov",    0xf006, 0xf10f, OP_AT_R0_REG_DREG },
  { "fmov.s",  0xf008, 0xf00f, OP_AT_REG_FREG },
  { "fmov.s",  0xf009, 0xf00f, OP_AT_REG_PLUS_FREG },
  { "fmov.s",  0xf006, 0xf00f, OP_AT_R0_REG_FREG },
  { "fmov",    0xf108, 0xf10f, OP_AT_REG_XDREG },
  { "fmov",    0xf109, 0xf10f, OP_AT_REG_PLUS_XDREG },
  { "fmov",    0xf106, 0xf10f, OP_AT_R0_REG_XDREG },
  { NULL,      0x0000, 0x0000, 0 },
};

struct _operand_type_sh4 operand_type_sh4[] =
{
  { 0,               0, 0,                0,  0 }, // OP_NONE
  { 1,         SH4_REG, 8,                0,  0 }, // OP_REG
  { 1,        SH4_FREG, 8,                0,  0 }, // OP_FREG
  { 1,        SH4_DREG, 9,                0,  0 }, // OP_DREG
  { 2,         SH4_REG, 4,          SH4_REG,  8 }, // OP_REG_REG
  { 2,        SH4_FREG, 4,         SH4_FREG,  8 }, // OP_FREG_FREG
  { 2,        SH4_DREG, 5,         SH4_DREG,  9 }, // OP_DREG_DREG
  { 2,        SH4_DREG, 5,        SH4_XDREG,  9 }, // OP_DREG_XDREG
  { 2,       SH4_XDREG, 5,         SH4_DREG,  9 }, // OP_XDREG_DREG
  { 2,       SH4_XDREG, 5,        SH4_XDREG,  9 }, // OP_XDREG_XDREG
  { 2,       SH4_FVREG, 8,        SH4_FVREG, 10 }, // OP_FVREG_FVREG
  { 2,               0, 0,                0,  0 }, // OP_IMM_REG
  { 2,               0, 0,                0,  0 }, // OP_IMM_R0
  { 2,               0, 0,                0,  0 }, // OP_IMM_AT_R0_GBR
  { 1,               0, 0,                0,  0 }, // OP_BRANCH_S9
  { 1,               0, 0,                0,  0 }, // OP_BRANCH_S13
  { 2,               0, 0,                0,  0 }, // OP_FREG_FPUL
  { 2,               0, 0,                0,  0 }, // OP_DREG_FPUL
  { 2,               0, 0,                0,  0 }, // OP_FPUL_FREG
  { 2,               0, 0,                0,  0 }, // OP_FPUL_DREG
  { 3,               0, 0,                0,  0 }, // OP_FR0_FREG_FREG
  { 2,        SH4_FREG, 4,       SH4_AT_REG,  8 }, // OP_FREG_AT_REG
  { 2,        SH4_DREG, 5,       SH4_AT_REG,  8 }, // OP_DREG_AT_REG
  { 2,        SH4_FREG, 4, SH4_AT_MINUS_REG,  8 }, // OP_FREG_AT_MINUS_REG
  { 2,        SH4_DREG, 5, SH4_AT_MINUS_REG,  8 }, // OP_DREG_AT_MINUS_REG
  { 2,        SH4_FREG, 4,    SH4_AT_R0_REG,  8 }, // OP_FREG_AT_R0_REG
  { 2,        SH4_DREG, 5,    SH4_AT_R0_REG,  8 }, // OP_DREG_AT_R0_REG
  { 2,       SH4_XDREG, 5,       SH4_AT_REG,  8 }, // OP_XDREG_AT_REG
  { 2,       SH4_XDREG, 5, SH4_AT_MINUS_REG,  8 }, // OP_XDREG_AT_MINUS_REG
  { 2,       SH4_XDREG, 5,    SH4_AT_R0_REG,  8 }, // OP_XDREG_AT_R0_REG
  { 2,      SH4_AT_REG, 4,         SH4_DREG,  9 }, // OP_AT_REG_DREG
  { 2, SH4_AT_REG_PLUS, 4,         SH4_DREG,  9 }, // OP_AT_REG_PLUS_DREG
  { 2,   SH4_AT_R0_REG, 4,         SH4_DREG,  9 }, // OP_AT_R0_REG_DREG
  { 2,      SH4_AT_REG, 4,         SH4_FREG,  8 }, // OP_AT_REG_FREG
  { 2, SH4_AT_REG_PLUS, 4,         SH4_FREG,  8 }, // OP_AT_REG_PLUS_FREG
  { 2,   SH4_AT_R0_REG, 4,         SH4_FREG,  8 }, // OP_AT_R0_REG_FREG
  { 2,      SH4_AT_REG, 4,        SH4_XDREG,  9 }, // OP_AT_REG_XDREG
  { 2, SH4_AT_REG_PLUS, 4,        SH4_XDREG,  9 }, // OP_AT_REG_PLUS_XDREG
  { 2,   SH4_AT_R0_REG, 4,        SH4_XDREG,  9 }, // OP_AT_R0_REG_XDREG
};

