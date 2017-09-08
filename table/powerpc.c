/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2017 by Michael Kohn
 *
 */

#include "table/powerpc.h"

struct _table_powerpc table_powerpc[] =
{
  // Aliases
  { "blr",    0x4e800020, 0xffffffff, OP_NONE, FLAG_NONE, 0, 0 },
  { "li",     0x38000000, 0xfc0f0000, OP_RD_SIMM, FLAG_NONE, 0, 0 },
  { "blt",    0x41800000, 0xffe30003, OP_BRANCH_COND_ALIAS, FLAG_NONE, 0, 0 },
  { "ble",    0x40810000, 0xffe30003, OP_BRANCH_COND_ALIAS, FLAG_NONE, 0, 0 },
  { "beq",    0x41820000, 0xffe30003, OP_BRANCH_COND_ALIAS, FLAG_NONE, 0, 0 },
  { "bge",    0x40800000, 0xffe30003, OP_BRANCH_COND_ALIAS, FLAG_NONE, 0, 0 },
  { "bgt",    0x41810000, 0xffe30003, OP_BRANCH_COND_ALIAS, FLAG_NONE, 0, 0 },
  { "bnl",    0x40800000, 0xffe30003, OP_BRANCH_COND_ALIAS, FLAG_NONE, 0, 0 },
  { "bne",    0x40820000, 0xffe30003, OP_BRANCH_COND_ALIAS, FLAG_NONE, 0, 0 },
  { "bng",    0x40810000, 0xffe30003, OP_BRANCH_COND_ALIAS, FLAG_NONE, 0, 0 },
  { "bso",    0x41830000, 0xffe30003, OP_BRANCH_COND_ALIAS, FLAG_NONE, 0, 0 },
  { "bns",    0x40830000, 0xffe30003, OP_BRANCH_COND_ALIAS, FLAG_NONE, 0, 0 },
  { "bun",    0x41830000, 0xffe30003, OP_BRANCH_COND_ALIAS, FLAG_NONE, 0, 0 },
  { "bnu",    0x40830000, 0xffe30003, OP_BRANCH_COND_ALIAS, FLAG_NONE, 0, 0 },

  // Instructions
  { "add",    0x7c000214, 0xfc0007fe, OP_RD_RA_RB, FLAG_DOT, 0, 0 },
  { "addo",   0x7c000614, 0xfc0007fe, OP_RD_RA_RB, FLAG_DOT, 0, 0 },
  { "addc",   0x7c000014, 0xfc0007fe, OP_RD_RA_RB, FLAG_DOT, 0, 0 },
  { "addco",  0x7c000414, 0xfc0007fe, OP_RD_RA_RB, FLAG_DOT, 0, 0 },
  { "adde",   0x7c000114, 0xfc0007fe, OP_RD_RA_RB, FLAG_DOT, 0, 0 },
  { "addeo",  0x7c000514, 0xfc0007fe, OP_RD_RA_RB, FLAG_DOT, 0, 0 },
  { "addi",   0x38000000, 0xfc000000, OP_RD_RA_SIMM, FLAG_NONE, 0, 0 },
  { "addic",  0x30000000, 0xfc000000, OP_RD_RA_SIMM, FLAG_NONE, 0, 0 },
  { "addic",  0x34000000, 0xfc000000, OP_RD_RA_SIMM, FLAG_REQUIRE_DOT, 0, 0 },
  { "addis",  0x3c000000, 0xfc000000, OP_RD_RA_SIMM, FLAG_NONE, 0, 0 },
  { "addme",  0x7c0001d4, 0xfc0007fe, OP_RD_RA, FLAG_DOT, 0, 0 },
  { "addmeo", 0x7c0005d4, 0xfc00fffe, OP_RD_RA, FLAG_DOT, 0, 0 },
  { "addze",  0x7c000194, 0xfc0007fe, OP_RD_RA, FLAG_DOT, 0, 0 },
  { "addzeo", 0x7c000594, 0xfc00fffe, OP_RD_RA, FLAG_DOT, 0, 0 },
  { "and",    0x7c000038, 0xfc0007fe, OP_RA_RS_RB, FLAG_DOT, 0, 0 },
  { "andi",   0x70000000, 0xfc000000, OP_RA_RS_UIMM, FLAG_REQUIRE_DOT, 0, 0 },
  { "andis",  0x74000000, 0xfc000000, OP_RA_RS_UIMM, FLAG_REQUIRE_DOT, 0, 0 },
  { "b",      0x48000000, 0xfc000003, OP_BRANCH, FLAG_NONE, 0, 0 },
  { "ba",     0x48000002, 0xfc000003, OP_JUMP, FLAG_NONE, 0, 0 },
  { "bl",     0x48000001, 0xfc000003, OP_BRANCH, FLAG_NONE, 0, 0 },
  { "bla",    0x48000003, 0xfc000003, OP_JUMP, FLAG_NONE, 0, 0 },
  { "bc",     0x40000000, 0xfc000003, OP_BRANCH_COND_BD, FLAG_NONE, 0, 0 },
  { "bca",    0x40000002, 0xfc000003, OP_JUMP_COND_BD, FLAG_NONE, 0, 0 },
  { "bcl",    0x40000001, 0xfc000003, OP_BRANCH_COND_BD, FLAG_NONE, 0, 0 },
  { "bcla",   0x40000003, 0xfc000003, OP_JUMP_COND_BD, FLAG_NONE, 0, 0 },
  { "bcctr",  0x4c000420, 0xfc00ffff, OP_BRANCH_COND, FLAG_NONE, 0, 0 },
  { "bcctrl", 0x4c000421, 0xfc00ffff, OP_BRANCH_COND, FLAG_NONE, 0, 0 },
  { "bclr",   0x4c000020, 0xfc00ffff, OP_BRANCH_COND, FLAG_NONE, 0, 0 },
  { "bclrl",  0x4c000021, 0xfc00ffff, OP_BRANCH_COND, FLAG_NONE, 0, 0 },
  { "cmpd",   0x7c200000, 0xfc6007ff, OP_CMP, FLAG_NONE, 0, 0 },
  { "cmpw",   0x7c000000, 0xfc6007ff, OP_CMP, FLAG_NONE, 0, 0 },
  { "cmpwi",  0x2c000000, 0xfc600000, OP_CMPI, FLAG_NONE, 0, 0 },
  { "cmpdi",  0x2c200000, 0xfc600000, OP_CMPI, FLAG_NONE, 0, 0 },
  { "cntlzw", 0x7c000034, 0xfc0007fe, OP_RA_RS, FLAG_DOT, 0, 0 },
  { "crand",  0x4c000202, 0xfc0007ff, OP_CRB_CRB_CRB, FLAG_NONE, 0, 0 },
  { "crandc", 0x4c000102, 0xfc0007ff, OP_CRB_CRB_CRB, FLAG_NONE, 0, 0 },
  { "creqv",  0x4c000242, 0xfc0007ff, OP_CRB_CRB_CRB, FLAG_NONE, 0, 0 },
  { "crnand", 0x4c0001c2, 0xfc0007ff, OP_CRB_CRB_CRB, FLAG_NONE, 0, 0 },
  { "crnor",  0x4c000042, 0xfc0007ff, OP_CRB_CRB_CRB, FLAG_NONE, 0, 0 },
  { "cror",   0x4c000382, 0xfc0007ff, OP_CRB_CRB_CRB, FLAG_NONE, 0, 0 },
  { "crorc",  0x4c000342, 0xfc0007ff, OP_CRB_CRB_CRB, FLAG_NONE, 0, 0 },
  { "crxor",  0x4c000182, 0xfc0007ff, OP_CRB_CRB_CRB, FLAG_NONE, 0, 0 },
  { "divw",   0x7c0003d6, 0xfc0007fe, OP_RD_RA_RB, FLAG_DOT, 0, 0 },
  { "divwo",  0x7c0007d6, 0xfc0007fe, OP_RD_RA_RB, FLAG_DOT, 0, 0 },
  { "divwu",  0x7c000396, 0xfc0007fe, OP_RD_RA_RB, FLAG_DOT, 0, 0 },
  { "divwuo", 0x7c000796, 0xfc0007fe, OP_RD_RA_RB, FLAG_DOT, 0, 0 },
  { "eieio",  0x7c0006ac, 0xffffffff, OP_NONE, FLAG_NONE, 0, 0 },
  { "eqv",    0x7c000238, 0xfc0007fe, OP_RA_RS_RB, FLAG_DOT, 0, 0 },
  { "extsb",  0x7c000774, 0xfc0007fe, OP_RA_RS, FLAG_DOT, 0, 0 },
  { "extsh",  0x7c000734, 0xfc0007fe, OP_RA_RS, FLAG_DOT, 0, 0 },
  { "lbz",    0x88000000, 0xfc000000, OP_RD_OFFSET_RA, FLAG_NONE, 0, 0 },
  { "lbzu",   0x8c000000, 0xfc000000, OP_RD_OFFSET_RA, FLAG_NONE, 0, 0 },
  { "lbzux",  0x7c0000ee, 0xfc0007ff, OP_RD_RA_RB, FLAG_NONE, 0, 0 },
  { "lbzx",   0x7c0000ae, 0xfc0007ff, OP_RD_RA_RB, FLAG_NONE, 0, 0 },
  { "lha",    0xa8000000, 0xfc000000, OP_RD_OFFSET_RA, FLAG_NONE, 0, 0 },
  { "lhau",   0xac000000, 0xfc000000, OP_RD_OFFSET_RA, FLAG_NONE, 0, 0 },
  { "lhaux",  0x7c0002ee, 0xfc0007ff, OP_RD_RA_RB, FLAG_NONE, 0, 0 },
  { "lhax",   0x7c0002ae, 0xfc0007ff, OP_RD_RA_RB, FLAG_NONE, 0, 0 },
  { "lhbrx",  0x7c00062c, 0xfc0007ff, OP_RD_RA_RB, FLAG_NONE, 0, 0 },
  { "lhz",    0xa0000000, 0xfc000000, OP_RD_OFFSET_RA, FLAG_NONE, 0, 0 },
  { "lhzu",   0xa4000000, 0xfc000000, OP_RD_OFFSET_RA, FLAG_NONE, 0, 0 },
  { "lhzux",  0x7c00026e, 0xfc0007ff, OP_RD_RA_RB, FLAG_NONE, 0, 0 },
  { "lhzx",   0x7c00022e, 0xfc0007ff, OP_RD_RA_RB, FLAG_NONE, 0, 0 },
  { "lmw",    0xb8000000, 0xfc000000, OP_RD_OFFSET_RA, FLAG_NONE, 0, 0 },
  { "lswi",   0x7c0004aa, 0xfc0007ff, OP_RD_RA_NB, FLAG_NONE, 0, 0 },
  { "lswx",   0x7c00042a, 0xfc0007ff, OP_RD_RA_RB, FLAG_NONE, 0, 0 },
  { "lwbrx",  0x7c00042c, 0xfc0007ff, OP_RD_RA_RB, FLAG_NONE, 0, 0 },
  { "lwz",    0x80000000, 0xfc000000, OP_RD_OFFSET_RA, FLAG_NONE, 0, 0 },
  { "lwzu",   0x84000000, 0xfc000000, OP_RD_OFFSET_RA, FLAG_NONE, 0, 0 },
  { "lwzux",  0x7c00006e, 0xfc0007ff, OP_RD_RA_RB, FLAG_NONE, 0, 0 },
  { "lwzx",   0x7c00002e, 0xfc0007ff, OP_RD_RA_RB, FLAG_NONE, 0, 0 },
  { "mcrf",   0x4c000000, 0xfc63ffff, OP_CRD_CRS, FLAG_NONE, 0, 0 },
  { "mcrxr",  0x7c000400, 0xfc7fffff, OP_CRD, FLAG_NONE, 0, 0 },
  { "mfcr",   0x7c000026, 0xfc1fffff, OP_RD, FLAG_NONE, 0, 0 },
  { "mfspr",  0x7c0002a6, 0xfc0007ff, OP_RD_SPR, FLAG_NONE, 0, 0 },
  { "mftb",   0x7c0002e6, 0xfc0007ff, OP_RD_TBR, FLAG_NONE, 0, 0 },
  { "mtcrf",  0x7c000120, 0xfc100fff, OP_CRM_RS, FLAG_NONE, 0, 0 },
  { "mtspr",  0x7c0003a6, 0xfc0007ff, OP_SPR_RS, FLAG_NONE, 0, 0 },
  { "mulhw",  0x7c000096, 0xfc0007fe, OP_RD_RA_RB, FLAG_DOT, 0, 0 },
  { "mulhwu", 0x7c000016, 0xfc0007fe, OP_RD_RA_RB, FLAG_DOT, 0, 0 },
  { "mulli",  0x1c000000, 0xfc000000, OP_RD_RA_SIMM, FLAG_NONE, 0, 0 },
  { "mullw",  0x7c0001d6, 0xfc0007fe, OP_RD_RA_RB, FLAG_DOT, 0, 0 },
  { "mullwo", 0x7c0005d6, 0xfc0007fe, OP_RD_RA_RB, FLAG_DOT, 0, 0 },
  { "nand",   0x7c0003b8, 0xfc0007fe, OP_RA_RS_RB, FLAG_DOT, 0, 0 },
  { "nor",    0x7c0000f8, 0xfc0007fe, OP_RA_RS_RB, FLAG_DOT, 0, 0 },
  { "orc",    0x7c000338, 0xfc0007fe, OP_RA_RS_RB, FLAG_DOT, 0, 0 },
  { "ori",    0x60000000, 0xfc000000, OP_RA_RS_UIMM, FLAG_NONE, 0, 0 },
  { "oris",   0x64000000, 0xfc000000, OP_RA_RS_UIMM, FLAG_NONE, 0, 0 },
  { "rlwimi", 0x50000000, 0xfc000000, OP_RA_RS_SH_MB_ME, FLAG_DOT, 0, 0 },
  { "rlwinm", 0x54000000, 0xfc000000, OP_RA_RS_SH_MB_ME, FLAG_DOT, 0, 0 },
  { "rlwnm",  0x5c000000, 0xfc000000, OP_RA_RS_SH_MB_ME, FLAG_DOT, 0, 0 },
  { "slw",    0x7c000030, 0xfc0007fe, OP_RA_RS_RB, FLAG_DOT, 0, 0 },
  { "sraw",   0x7c000630, 0xfc0007fe, OP_RA_RS_RB, FLAG_DOT, 0, 0 },
  { "srawi",  0x7c000670, 0xfc0007fe, OP_RA_RS_SH, FLAG_DOT, 0, 0 },
  { "srw",    0x7c000430, 0xfc0007fe, OP_RA_RS_RB, FLAG_DOT, 0, 0 },
  { "stb",    0x98000000, 0xfc000000, OP_RS_OFFSET_RA, FLAG_NONE, 0, 0 },
  { "stbu",   0x9c000000, 0xfc000000, OP_RS_OFFSET_RA, FLAG_NONE, 0, 0 },
  { "stbux",  0x7c0001ee, 0xfc0007ff, OP_RS_RA_RB, FLAG_NONE, 0, 0 },
  { "stbx",   0x7c0001ae, 0xfc0007ff, OP_RS_RA_RB, FLAG_NONE, 0, 0 },
  { "sth",    0xb0000000, 0xfc000000, OP_RS_OFFSET_RA, FLAG_NONE, 0, 0 },
  { "sthbrx", 0x7c00072c, 0xfc0007ff, OP_RS_RA_RB, FLAG_NONE, 0, 0 },
  { "sthu",   0xb4000000, 0xfc000000, OP_RS_OFFSET_RA, FLAG_NONE, 0, 0 },
  { "sthux",  0x7c00036e, 0xfc0007ff, OP_RS_RA_RB, FLAG_NONE, 0, 0 },
  { "sthx",   0x7c00032e, 0xfc0007ff, OP_RS_RA_RB, FLAG_NONE, 0, 0 },
  { "stmw",   0xbc000000, 0xfc000000, OP_RS_OFFSET_RA, FLAG_NONE, 0, 0 },
  { "stswi",  0x7c0005aa, 0xfc0007ff, OP_RS_RA_NB, FLAG_NONE, 0, 0 },
  { "stswx",  0x7c00052a, 0xfc0007ff, OP_RS_RA_RB, FLAG_NONE, 0, 0 },
  { "stw",    0x90000000, 0xfc000000, OP_RS_OFFSET_RA, FLAG_NONE, 0, 0 },
  { "stwbrx", 0x7c00052c, 0xfc0007ff, OP_RS_RA_RB, FLAG_NONE, 0, 0 },
  { "stwu",   0x94000000, 0xfc000000, OP_RS_OFFSET_RA, FLAG_NONE, 0, 0 },
  { "stwux",  0x7c00016e, 0xfc0007ff, OP_RS_RA_RB, FLAG_NONE, 0, 0 },
  { "stwx",   0x7c00012e, 0xfc0007ff, OP_RS_RA_RB, FLAG_NONE, 0, 0 },
  { "subf",   0x7c000050, 0xfc0007fe, OP_RD_RA_RB, FLAG_DOT, 0, 0 },
  { "subfo",  0x7c000450, 0xfc0007fe, OP_RD_RA_RB, FLAG_DOT, 0, 0 },
  { "subfc",  0x7c000010, 0xfc0007fe, OP_RD_RA_RB, FLAG_DOT, 0, 0 },
  { "subfco", 0x7c000410, 0xfc0007fe, OP_RD_RA_RB, FLAG_DOT, 0, 0 },
  { "subfe",  0x7c000110, 0xfc0007fe, OP_RD_RA_RB, FLAG_DOT, 0, 0 },
  { "subfeo", 0x7c000510, 0xfc0007fe, OP_RD_RA_RB, FLAG_DOT, 0, 0 },
  { "subfic", 0x20000000, 0xfc000000, OP_RD_RA_SIMM, FLAG_NONE, 0, 0 },
  { "subfme", 0x7c0001d0, 0xfc00fffe, OP_RD_RA, FLAG_DOT, 0, 0 },
  { "subfmeo",0x7c0005d0, 0xfc00fffe, OP_RD_RA, FLAG_DOT, 0, 0 },
  { "subfze", 0x7c000190, 0xfc00fffe, OP_RD_RA, FLAG_DOT, 0, 0 },
  { "subfzeo",0x7c000590, 0xfc00fffe, OP_RD_RA, FLAG_DOT, 0, 0 },
  { "sync",   0x7c0004ac, 0xffffffff, OP_NONE, FLAG_NONE, 0, 0 },
  { "xor",    0x7c000278, 0xfc0007fe, OP_RA_RS_RB, FLAG_DOT, 0, 0 },
  { "xori",   0x68000000, 0xfc000000, OP_RA_RS_UIMM, FLAG_NONE, 0, 0 },
  { "xoris",  0x6c000000, 0xfc000000, OP_RA_RS_UIMM, FLAG_NONE, 0, 0 },

  // Altivec
  { "dss",    0x7c00066c, 0xff9fffff, OP_STRM, FLAG_NONE, 0, 0 },
  { "dssall", 0x7e00066c, 0xffffffff, OP_NONE, FLAG_NONE, 0, 0 },
  { "dst",    0x7c0002ac, 0xff8007ff, OP_RA_RB_STRM, FLAG_NONE, 0, 0 },
  { "dstst",  0x7c0002ec, 0xff8007ff, OP_RA_RB_STRM, FLAG_NONE, 0, 0 },
  { "dststt", 0x7e0002ec, 0xff8007ff, OP_RA_RB_STRM, FLAG_NONE, 0, 0 },
  { "dstt",   0x7e0002ac, 0xff8007ff, OP_RA_RB_STRM, FLAG_NONE, 0, 0 }, // ??
  { "lvebx",  0x7c00000e, 0xfc0007ff, OP_VD_RA_RB, FLAG_NONE, 0, 0 },
  { "lvehx",  0x7c00004e, 0xfc0007ff, OP_VD_RA_RB, FLAG_NONE, 0, 0 },
  { "lvewx",  0x7c00008e, 0xfc0007ff, OP_VD_RA_RB, FLAG_NONE, 0, 0 },
  { "lvsl",   0x7c00000c, 0xfc0007ff, OP_VD_RA_RB, FLAG_NONE, 0, 0 },
  { "lvsr",   0x7c00004c, 0xfc0007ff, OP_VD_RA_RB, FLAG_NONE, 0, 0 },
  { "lvx",    0x7c0000ce, 0xfc0007ff, OP_VD_RA_RB, FLAG_NONE, 0, 0 },
  { "lvxl",   0x7c0002ce, 0xfc0007ff, OP_VD_RA_RB, FLAG_NONE, 0, 0 },
  { "mfvscr", 0x10000604, 0xfc1fffff, OP_VD, FLAG_NONE, 0, 0 },
  { "mtvscr", 0x10000644, 0xffff07ff, OP_VB, FLAG_NONE, 0, 0 },
  { "stvebx", 0x7c00010e, 0xfc0007ff, OP_VD_RA_RB, FLAG_NONE, 0, 0 },
  { "stvehx", 0x7c00014e, 0xfc0007ff, OP_VD_RA_RB, FLAG_NONE, 0, 0 },
  { "stvewx", 0x7c00018e, 0xfc0007ff, OP_VD_RA_RB, FLAG_NONE, 0, 0 },
  { "stvx",   0x7c0001ce, 0xfc0007ff, OP_VD_RA_RB, FLAG_NONE, 0, 0 },
  { "stvxl",  0x7c0003ce, 0xfc0007ff, OP_VD_RA_RB, FLAG_NONE, 0, 0 },
  { "vaddcuw",0x10000180, 0xfc0007ff, OP_VD_VA_VB, FLAG_NONE, 0, 0 },
  { "vaddfp", 0x1000000a, 0xfc0007ff, OP_VD_VA_VB, FLAG_NONE, 0, 0 },
  { "vaddsbs",0x10000300, 0xfc0007ff, OP_VD_VA_VB, FLAG_NONE, 0, 0 },
  { "vaddshs",0x10000340, 0xfc0007ff, OP_VD_VA_VB, FLAG_NONE, 0, 0 },
  { "vaddsws",0x10000380, 0xfc0007ff, OP_VD_VA_VB, FLAG_NONE, 0, 0 },
  { "vaddubm",0x10000000, 0xfc0007ff, OP_VD_VA_VB, FLAG_NONE, 0, 0 },
  { "vaddubs",0x10000200, 0xfc0007ff, OP_VD_VA_VB, FLAG_NONE, 0, 0 },
  { "vadduhm",0x10000040, 0xfc0007ff, OP_VD_VA_VB, FLAG_NONE, 0, 0 },
  { "vadduhs",0x10000240, 0xfc0007ff, OP_VD_VA_VB, FLAG_NONE, 0, 0 },
  { "vadduwm",0x10000080, 0xfc0007ff, OP_VD_VA_VB, FLAG_NONE, 0, 0 },
  { "vadduws",0x10000280, 0xfc0007ff, OP_VD_VA_VB, FLAG_NONE, 0, 0 },
  { "vand",   0x10000404, 0xfc0007ff, OP_VD_VA_VB, FLAG_NONE, 0, 0 },
  { "vandc",  0x10000444, 0xfc0007ff, OP_VD_VA_VB, FLAG_NONE, 0, 0 },
  { "vavgsb", 0x10000502, 0xfc0007ff, OP_VD_VA_VB, FLAG_NONE, 0, 0 },
  { "vavgsh", 0x10000542, 0xfc0007ff, OP_VD_VA_VB, FLAG_NONE, 0, 0 },
  { "vavgsw", 0x10000582, 0xfc0007ff, OP_VD_VA_VB, FLAG_NONE, 0, 0 },
  { "vavgub", 0x10000402, 0xfc0007ff, OP_VD_VA_VB, FLAG_NONE, 0, 0 },
  { "vavguh", 0x10000442, 0xfc0007ff, OP_VD_VA_VB, FLAG_NONE, 0, 0 },
  { "vavguw", 0x10000482, 0xfc0007ff, OP_VD_VA_VB, FLAG_NONE, 0, 0 },
  { "vcfsx",     0x1000034a, 0xfc0007ff, OP_VD_VB_UIMM, FLAG_NONE, 0, 0 },
  { "vcfux",     0x1000030a, 0xfc0007ff, OP_VD_VB_UIMM, FLAG_NONE, 0, 0 },
  { "vcmpbfpx",  0x100003c6, 0xfc0003ff, OP_VD_VA_VB, FLAG_DOT, 0, 0 },
  { "vcmpeqfpx", 0x100000c6, 0xfc0003ff, OP_VD_VA_VB, FLAG_DOT, 0, 0 },
  { "vcmpequbx", 0x10000006, 0xfc0003ff, OP_VD_VA_VB, FLAG_DOT, 0, 0 },
  { "vcmpequhx", 0x10000046, 0xfc0003ff, OP_VD_VA_VB, FLAG_DOT, 0, 0 },
  { "vcmpequwx", 0x10000086, 0xfc0003ff, OP_VD_VA_VB, FLAG_DOT, 0, 0 },
  { "vcmpgefpx", 0x100001c6, 0xfc0003ff, OP_VD_VA_VB, FLAG_DOT, 0, 0 },
  { "vcmpgtfpx", 0x100002c6, 0xfc0003ff, OP_VD_VA_VB, FLAG_DOT, 0, 0 },
  { "vcmpgtsbx", 0x10000306, 0xfc0003ff, OP_VD_VA_VB, FLAG_DOT, 0, 0 },
  { "vcmpgtshx", 0x10000346, 0xfc0003ff, OP_VD_VA_VB, FLAG_DOT, 0, 0 },
  { "vcmpgtswx", 0x10000386, 0xfc0003ff, OP_VD_VA_VB, FLAG_DOT, 0, 0 },
  { "vcmpgtubx", 0x10000206, 0xfc0003ff, OP_VD_VA_VB, FLAG_DOT, 0, 0 },
  { "vcmpgtuhx", 0x10000246, 0xfc0003ff, OP_VD_VA_VB, FLAG_DOT, 0, 0 },
  { "vcmpgtuwx", 0x10000286, 0xfc0003ff, OP_VD_VA_VB, FLAG_DOT, 0, 0 },
  { "vctsxs",    0x100003ca, 0xfc0007ff, OP_VD_VB_UIMM, FLAG_NONE, 0, 0 },
  { "vctuxs",    0x1000038a, 0xfc0007ff, OP_VD_VB_UIMM, FLAG_NONE, 0, 0 },
  { "vexptefp",  0x1000018a, 0xfc1f07ff, OP_VD_VB, FLAG_NONE, 0, 0 },
  { "vlogefp",   0x100001ca, 0xfc1f07ff, OP_VD_VB, FLAG_NONE, 0, 0 },
  { "vmaddfp",   0x1000002e, 0xfc00003f, OP_VD_VA_VC_VB, FLAG_NONE, 0, 0 },
  { "vmaxfp",    0x1000040a, 0xfc0007ff, OP_VD_VA_VB, FLAG_NONE, 0, 0 },
  { "vmaxsb",    0x10000102, 0xfc0007ff, OP_VD_VA_VB, FLAG_NONE, 0, 0 },
  { "vmaxsh",    0x10000142, 0xfc0007ff, OP_VD_VA_VB, FLAG_NONE, 0, 0 },
  { "vmaxsw",    0x10000182, 0xfc0007ff, OP_VD_VA_VB, FLAG_NONE, 0, 0 },
  { "vmaxub",    0x10000002, 0xfc0007ff, OP_VD_VA_VB, FLAG_NONE, 0, 0 },
  { "vmaxuh",    0x10000042, 0xfc0007ff, OP_VD_VA_VB, FLAG_NONE, 0, 0 },
  { "vmaxuw",    0x10000082, 0xfc0007ff, OP_VD_VA_VB, FLAG_NONE, 0, 0 },
  { "vmhaddshs", 0x10000020, 0xfc00003f, OP_VD_VA_VB_VC, FLAG_NONE, 0, 0 },
  { "vmhraddshs",0x10000021, 0xfc00003f, OP_VD_VA_VB_VC, FLAG_NONE, 0, 0 },
  { "vminfp",    0x1000044a, 0xfc0007ff, OP_VD_VA_VB, FLAG_NONE, 0, 0 },
  { "vminsb",    0x10000302, 0xfc0007ff, OP_VD_VA_VB, FLAG_NONE, 0, 0 },
  { "vminsh",    0x10000342, 0xfc0007ff, OP_VD_VA_VB, FLAG_NONE, 0, 0 },
  { "vminsw",    0x10000382, 0xfc0007ff, OP_VD_VA_VB, FLAG_NONE, 0, 0 },
  { "vminub",    0x10000202, 0xfc0007ff, OP_VD_VA_VB, FLAG_NONE, 0, 0 },
  { "vminuh",    0x10000242, 0xfc0007ff, OP_VD_VA_VB, FLAG_NONE, 0, 0 },
  { "vminuw",    0x10000282, 0xfc0007ff, OP_VD_VA_VB, FLAG_NONE, 0, 0 },
  { "vmladduhm", 0x10000022, 0xfc00003f, OP_VD_VA_VB_VC, FLAG_NONE, 0, 0 },
  { "vmrghb",    0x1000000c, 0xfc0007ff, OP_VD_VA_VB, FLAG_NONE, 0, 0 },
  { "vmrghh",    0x1000004c, 0xfc0007ff, OP_VD_VA_VB, FLAG_NONE, 0, 0 },
  { "vmrghw",    0x1000008c, 0xfc0007ff, OP_VD_VA_VB, FLAG_NONE, 0, 0 },
  { "vmrglb",    0x1000010c, 0xfc0007ff, OP_VD_VA_VB, FLAG_NONE, 0, 0 },
  { "vmrglh",    0x1000014c, 0xfc0007ff, OP_VD_VA_VB, FLAG_NONE, 0, 0 },
  { "vmrglw",    0x1000018c, 0xfc0007ff, OP_VD_VA_VB, FLAG_NONE, 0, 0 },
  { "vmsummbm",  0x10000025, 0xfc00003f, OP_VD_VA_VB_VC, FLAG_NONE, 0, 0 },
  { "vmsumshm",  0x10000028, 0xfc00003f, OP_VD_VA_VB_VC, FLAG_NONE, 0, 0 },
  { "vmsumshs",  0x10000029, 0xfc00003f, OP_VD_VA_VB_VC, FLAG_NONE, 0, 0 },
  { "vmsumubm",  0x10000024, 0xfc00003f, OP_VD_VA_VB_VC, FLAG_NONE, 0, 0 },
  { "vmsumuhm",  0x10000026, 0xfc00003f, OP_VD_VA_VB_VC, FLAG_NONE, 0, 0 },
  { "vmsumuhs",  0x10000027, 0xfc00003f, OP_VD_VA_VB_VC, FLAG_NONE, 0, 0 },
  { "vmulesb",   0x10000308, 0xfc0007ff, OP_VD_VA_VB, FLAG_NONE, 0, 0 },
  { "vmulesh",   0x10000348, 0xfc0007ff, OP_VD_VA_VB, FLAG_NONE, 0, 0 },
  { "vmuleub",   0x10000208, 0xfc0007ff, OP_VD_VA_VB, FLAG_NONE, 0, 0 },
  { "vmuleuh",   0x10000248, 0xfc0007ff, OP_VD_VA_VB, FLAG_NONE, 0, 0 },
  { "vmulosb",   0x10000108, 0xfc0007ff, OP_VD_VA_VB, FLAG_NONE, 0, 0 },
  { "vmulosh",   0x10000148, 0xfc0007ff, OP_VD_VA_VB, FLAG_NONE, 0, 0 },
  { "vmuloub",   0x10000008, 0xfc0007ff, OP_VD_VA_VB, FLAG_NONE, 0, 0 },
  { "vmulouh",   0x10000048, 0xfc0007ff, OP_VD_VA_VB, FLAG_NONE, 0, 0 },
  { "vnmsubfp",  0x1000002f, 0xfc00003f, OP_VD_VA_VC_VB, FLAG_NONE, 0, 0 },
  { "vnor",      0x10000504, 0xfc0007ff, OP_VD_VA_VB, FLAG_NONE, 0, 0 },
  { "vor",       0x10000484, 0xfc0007ff, OP_VD_VA_VB, FLAG_NONE, 0, 0 },
  { "vperm",     0x1000002b, 0xfc00003f, OP_VD_VA_VB_VC, FLAG_NONE, 0, 0 },
  { "vpkpx",     0x1000030e, 0xfc0007ff, OP_VD_VA_VB, FLAG_NONE, 0, 0 },
  { "vpkshss",   0x1000018e, 0xfc0007ff, OP_VD_VA_VB, FLAG_NONE, 0, 0 },
  { "vpkshus",   0x1000010e, 0xfc0007ff, OP_VD_VA_VB, FLAG_NONE, 0, 0 },
  { "vpkswss",   0x100001ce, 0xfc0007ff, OP_VD_VA_VB, FLAG_NONE, 0, 0 },
  { "vpkswus",   0x1000014e, 0xfc0007ff, OP_VD_VA_VB, FLAG_NONE, 0, 0 },
  { "vpkuhum",   0x1000000e, 0xfc0007ff, OP_VD_VA_VB, FLAG_NONE, 0, 0 },
  { "vpkuhus",   0x1000008e, 0xfc0007ff, OP_VD_VA_VB, FLAG_NONE, 0, 0 },
  { "vpkuwum",   0x1000004e, 0xfc0007ff, OP_VD_VA_VB, FLAG_NONE, 0, 0 },
  { "vpkuwus",   0x100000ce, 0xfc0007ff, OP_VD_VA_VB, FLAG_NONE, 0, 0 },
  { "vrefp",     0x1000010a, 0xfc1f07ff, OP_VD_VB, FLAG_NONE, 0, 0 },
  { "vrfim",     0x100002ca, 0xfc1f07ff, OP_VD_VB, FLAG_NONE, 0, 0 },
  { "vrfin",     0x1000020a, 0xfc1f07ff, OP_VD_VB, FLAG_NONE, 0, 0 },
  { "vrfip",     0x1000028a, 0xfc1f07ff, OP_VD_VB, FLAG_NONE, 0, 0 },
  { "vrfiz",     0x1000024a, 0xfc1f07ff, OP_VD_VB, FLAG_NONE, 0, 0 },
  { "vrlb",      0x10000004, 0xfc0007ff, OP_VD_VA_VB, FLAG_NONE, 0, 0 },
  { "vrlh",      0x10000044, 0xfc0007ff, OP_VD_VA_VB, FLAG_NONE, 0, 0 },
  { "vrlw",      0x10000084, 0xfc0007ff, OP_VD_VA_VB, FLAG_NONE, 0, 0 },
  { "vrsqrtefp", 0x1000014a, 0xfc1f07ff, OP_VD_VB, FLAG_NONE, 0, 0 },
  { "vsel",      0x1000002a, 0xfc00003f, OP_VD_VA_VB_VC, FLAG_NONE, 0, 0 },
  { "vsl",       0x100001c4, 0xfc0007ff, OP_VD_VA_VB, FLAG_NONE, 0, 0 },
  { "vslb",      0x10000104, 0xfc0007ff, OP_VD_VA_VB, FLAG_NONE, 0, 0 },
  { "vsldoi",    0x1000002c, 0xfc00043f, OP_VD_VA_VB_SH, FLAG_NONE, 0, 0 },
  { "vslh",      0x10000144, 0xfc0007ff, OP_VD_VA_VB, FLAG_NONE, 0, 0 },
  { "vslo",      0x1000040c, 0xfc0007ff, OP_VD_VA_VB, FLAG_NONE, 0, 0 },
  { "vslw",      0x10000184, 0xfc0007ff, OP_VD_VA_VB, FLAG_NONE, 0, 0 },
  { "vspltb",    0x1000020c, 0xfc0007ff, OP_VD_VB_UIMM, FLAG_NONE, 0, 0 },
  { "vsplth",    0x1000024c, 0xfc0007ff, OP_VD_VB_UIMM, FLAG_NONE, 0, 0 },
  { "vspltisb",  0x1000030c, 0xfc0007ff, OP_VD_SIMM, FLAG_NONE, 0, 0 },
  { "vspltish",  0x1000034c, 0xfc0007ff, OP_VD_SIMM, FLAG_NONE, 0, 0 },
  { "vspltisw",  0x1000038c, 0xfc0007ff, OP_VD_SIMM, FLAG_NONE, 0, 0 },
  { "vspltw",    0x1000028c, 0xfc0007ff, OP_VD_VB_UIMM, FLAG_NONE, 0, 0 },
  { "vsr",       0x100002c4, 0xfc0007ff, OP_VD_VA_VB, FLAG_NONE, 0, 0 },
  { "vsrab",     0x10000304, 0xfc0007ff, OP_VD_VA_VB, FLAG_NONE, 0, 0 },
  { "vsrah",     0x10000344, 0xfc0007ff, OP_VD_VA_VB, FLAG_NONE, 0, 0 },
  { "vsraw",     0x10000384, 0xfc0007ff, OP_VD_VA_VB, FLAG_NONE, 0, 0 },
  { "vsrb",      0x10000204, 0xfc0007ff, OP_VD_VA_VB, FLAG_NONE, 0, 0 },
  { "vsrh",      0x10000244, 0xfc0007ff, OP_VD_VA_VB, FLAG_NONE, 0, 0 },
  { "vsro",      0x1000044c, 0xfc0007ff, OP_VD_VA_VB, FLAG_NONE, 0, 0 },
  { "vsrw",      0x10000284, 0xfc0007ff, OP_VD_VA_VB, FLAG_NONE, 0, 0 },
  { "vsubcuw",   0x10000580, 0xfc0007ff, OP_VD_VA_VB, FLAG_NONE, 0, 0 },
  { "vsubfp",    0x1000004a, 0xfc0007ff, OP_VD_VA_VB, FLAG_NONE, 0, 0 },
  { "vsubsbs",   0x10000700, 0xfc0007ff, OP_VD_VA_VB, FLAG_NONE, 0, 0 },
  { "vsubshs",   0x10000740, 0xfc0007ff, OP_VD_VA_VB, FLAG_NONE, 0, 0 },
  { "vsubsws",   0x10000780, 0xfc0007ff, OP_VD_VA_VB, FLAG_NONE, 0, 0 },
  { "vsububm",   0x10000400, 0xfc0007ff, OP_VD_VA_VB, FLAG_NONE, 0, 0 },
  { "vsububs",   0x10000600, 0xfc0007ff, OP_VD_VA_VB, FLAG_NONE, 0, 0 },
  { "vsubuhm",   0x10000440, 0xfc0007ff, OP_VD_VA_VB, FLAG_NONE, 0, 0 },
  { "vsubuhs",   0x10000640, 0xfc0007ff, OP_VD_VA_VB, FLAG_NONE, 0, 0 },
  { "vsubuwm",   0x10000480, 0xfc0007ff, OP_VD_VA_VB, FLAG_NONE, 0, 0 },
  { "vsubuws",   0x10000680, 0xfc0007ff, OP_VD_VA_VB, FLAG_NONE, 0, 0 },
  { "vsumsws",   0x10000788, 0xfc0007ff, OP_VD_VA_VB, FLAG_NONE, 0, 0 },
  { "vsum2sws",  0x10000688, 0xfc0007ff, OP_VD_VA_VB, FLAG_NONE, 0, 0 },
  { "vsum4sbs",  0x10000708, 0xfc0007ff, OP_VD_VA_VB, FLAG_NONE, 0, 0 },
  { "vsum4shs",  0x10000648, 0xfc0007ff, OP_VD_VA_VB, FLAG_NONE, 0, 0 },
  { "vsum4ubs",  0x10000608, 0xfc0007ff, OP_VD_VA_VB, FLAG_NONE, 0, 0 },
  { "vupkhpx",   0x1000034e, 0xfc1f07ff, OP_VD_VB, FLAG_NONE, 0, 0 },
  { "vupkhsb",   0x1000020e, 0xfc1f07ff, OP_VD_VB, FLAG_NONE, 0, 0 },
  { "vupkhsh",   0x1000024e, 0xfc1f07ff, OP_VD_VB, FLAG_NONE, 0, 0 },
  { "vupklpx",   0x100003ce, 0xfc1f07ff, OP_VD_VB, FLAG_NONE, 0, 0 },
  { "vupklsb",   0x1000028e, 0xfc1f07ff, OP_VD_VB, FLAG_NONE, 0, 0 },
  { "vupklsh",   0x100002ce, 0xfc1f07ff, OP_VD_VB, FLAG_NONE, 0, 0 },
  { "vxor",      0x100004c4, 0xfc0007ff, OP_VD_VA_VB, FLAG_NONE, 0, 0 },
  { NULL, 0, 0, 0, 0 }
};

struct _powerpc_spr powerpc_spr[] =
{
  { "xer",    1 },
  { "lr",     8 },
  { "ctr",    9 },
  { "dsisr",  18 },
  { "dar",    19 },
  { "dec",    22 },
  { "sdr1",   25 },
  { "srr0",   26 },
  { "srr1",   27 },
  { "sprg0",  272 },
  { "sprg1",  273 },
  { "sprg2",  274 },
  { "sprg3",  275 },
  { "asr",    280 },
  { "ear",    282 },
  { "pvr",    287 },
  { "ibat0u", 528 },
  { "ibat0l", 529 },
  { "ibat1u", 530 },
  { "ibat1l", 531 },
  { "ibat2u", 532 },
  { "ibat2l", 533 },
  { "ibat3u", 534 },
  { "ibat3l", 535 },
  { "dbat0u", 536 },
  { "dbat0l", 537 },
  { "dbat1u", 538 },
  { "dbat1l", 539 },
  { "dbat2u", 540 },
  { "dbat2l", 541 },
  { "dbat3u", 542 },
  { "dbat3l", 543 },
  { "dabr",   1013 },
  { NULL,     -1 }
};


