/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2022 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "table/arc.h"

struct _table_arc table_arc[] =
{
#if 0
  { "bbit0",  0x0801000e, 0xf801001f, OP_B_C_O9, F_D },
  { "bbit0",  0x0801001e, 0xf801001f, OP_B_U6_O9, F_D },
  { "bbit1",  0x0801000f, 0xf801001f, OP_B_C_O9, F_D },
  { "bbit1",  0x0801001f, 0xf801001f, OP_B_U6_O9, F_D },
#endif
  { NULL, 0, 0, 0, 0 },
};

struct _table_arc16 table_arc16[] =
{
  { "abs_s",  0x7811, 0xf81f, OP_B_C },
  { "add_s",  0x6018, 0xf818, OP_A_B_C },
  { "add_s",  0x6800, 0xf818, OP_C_B_U3 },
  { "add_s",  0x70c7, 0xf8ff, OP_B_B_LIMM },
  { "add_s",  0x7000, 0xf818, OP_B_B_H },
  { "add_s",  0xc080, 0xf8e0, OP_B_SP_U7 },
  { "add_s",  0xc0a0, 0xffe0, OP_SP_SP_U7 },
  { "add_s",  0xce00, 0xfe00, OP_R0_GP_S9 },
  { "add_s",  0xe000, 0xf880, OP_B_B_U7 },
  { "add1_s", 0x7814, 0xf81f, OP_B_B_C },
  { "add2_s", 0x7815, 0xf81f, OP_B_B_C },
  { "add3_s", 0x7816, 0xf81f, OP_B_B_C },
  { "and_s",  0x7804, 0xf81f, OP_B_B_C },
  { "asl_s",  0x781b, 0xf81f, OP_B_C },
  { "asl_s",  0x6810, 0xf818, OP_C_B_U3 },
  { "asl_s",  0x7818, 0xf81f, OP_B_B_C },
  { "asl_s",  0xb800, 0xf8e0, OP_B_B_U5 },
  { "asr_s",  0x781c, 0xf81f, OP_B_C },
  { "asr_s",  0x6818, 0xf818, OP_C_B_U3 },
  { "asr_s",  0x781a, 0xf81f, OP_B_B_C },
  { "asr_s",  0xb840, 0xf8e0, OP_B_B_U5 },
  { "bclr_s", 0xb8a0, 0xf8e0, OP_B_B_U5 },
  { "bic_s",  0x7806, 0xf81f, OP_B_B_C },
  { "bmsk_s", 0xb8c0, 0xf8e0, OP_B_B_U5 },
  { "brk_s",  0x7fff, 0xffff, OP_NONE },
  { "bset_s", 0xb880, 0xf8e0, OP_B_B_U5 },
  { "btst_s", 0xb880, 0xb8e0, OP_B_U5 },
  { "cmp_s",  0x7010, 0xf818, OP_B_H },
  { "cmp_s",  0x70d7, 0xf8ff, OP_B_LIMM },
  { "cmp_s",  0xe080, 0xf880, OP_B_U7 },
  { "extb_s", 0x780f, 0xf81f, OP_B_C },
  { "extw_s", 0x7810, 0xf81f, OP_B_C },
  { "lsr_s",  0x781d, 0xf81f, OP_B_C },
  { "lsr_s",  0x7819, 0xf81f, OP_B_B_C },
  { "lsr_s",  0xb820, 0xf8e0, OP_B_B_U5 },
  { "mov_s",  0x7008, 0xf818, OP_B_H },
  { "mov_s",  0x70cf, 0xf8ff, OP_B_LIMM },
  { "mov_s",  0x7018, 0xf818, OP_HOB },
  { "mov_s",  0xd800, 0xf800, OP_B_U8 },
  { "neg_s",  0x7813, 0xf81f, OP_B_C },
  { "nop_s",  0x78e0, 0xffff, OP_NONE },
  { "not_s",  0x7812, 0xf81f, OP_B_C },
  { "or_s",   0x7805, 0xf81f, OP_B_B_C },
  { "sexw_s", 0x780e, 0xf81f, OP_B_C },
  { "sub_s",  0x6808, 0xf818, OP_C_B_U3 },
  { "sub_s",  0x6802, 0xf81f, OP_B_B_C },
  { "sub_s",  0xb860, 0xf8e0, OP_B_B_U5 },
  //{ "sub_s.ne", 0x78c0, 0xf8ff, OP_B_B_B },
  //{ "sub_s",   0xc1a0, 0xffe0, OP_SP_SP_U7 },
  //{ "trap_s",  0x788e, 0xf88f, OP_U6 },
  { "tst_s",  0x780b, 0xf81f, OP_B_C },
  { "unimp_s",0x79e0, 0xffff, OP_NONE },
  { "xor_s",  0x7807, 0xf81f, OP_B_B_C },

#if 0
  { "pop_s",  0xc0c1, 0xf8ff, OP_B },
  { "pop_s",  0xc0d1, 0xf8ff, OP_BLINK },
  { "push_s", 0xc0e1, 0xf8ff, OP_B },
  { "push_s", 0xc0e1, 0xf8ff, OP_BLINK },

  // FIXME: Change the format so all the operands are seprated and probably
  //        combine with 32 bit instructions.
  { "ld_s",   0x6000, 0xf818, OP_A_PAREN_B_C },
  { "ldb_s",  0x6008, 0xf818, OP_A_PAREN_B_C },
  { "ldw_s",  0x6010, 0xf818, OP_A_PAREN_B_C },
  { "ld_s",   0x8000, 0xf800, OP_C_PAREN_B_U7 },
  { "ldb_s",  0x8800, 0xf800, OP_C_PAREN_B_U5 },
  { "ldw_s",  0x9000, 0xf800, OP_C_PAREN_B_U6 },
  { "ldw_s.x",0x9800, 0xf800, OP_C_PAREN_B_U6 },
  { "ld_s",   0xc000, 0xf8e0, OP_B_PAREN_SP_U7 },
  { "ldb_s",  0xc020, 0xf8e0, OP_B_PAREN_SP_U7 },
  { "ld_s",   0xc800, 0xfe00, OP_R0_PAREN_GP_S11 },
  { "ldb_s",  0xca00, 0xfe00, OP_R0_PAREN_GP_S9 },
  { "ldw_s",  0xcc00, 0xfe00, OP_R0_PAREN_GP_S10 },
  { "ld_s",   0xd000, 0xf800, OP_B_PAREN_PCL_U10 },

  { "st_s",   0xa000, 0xf800, OP_C_PAREN_B_U7 },
  { "stb_s",  0xa800, 0xf800, OP_C_PAREN_B_U5 },
  { "stw_s",  0xb000, 0xf800, OP_C_PAREN_B_U6 },
  { "st_s",   0xc040, 0xf8e0, OP_B_PAREN_SP_U7 },
  { "stb_s",  0xc060, 0xf8e0, OP_B_PAREN_SP_U7 },
#endif

  { NULL, 0, 0, 0 },
};

struct _table_arc_op table_arc_alu[] =
{
  { "add",   0x00 },
  { "adc",   0x01 },
  { "sub",   0x02 },
  { "sbc",   0x03 },
  { "and",   0x04 },
  { "or",    0x05 },
  { "bic",   0x06 },
  { "xor",   0x07 },
  { "max",   0x08 },
  { "min",   0x09 },
  { "mov",   0x0a },
  { "tst",   0x0b },
  { "cmp",   0x0c },
  { "rcmp",  0x0d },
  { "rsub",  0x0e },
  { "bset",  0x0f },
  { "bclr",  0x10 },
  { "btst",  0x11 },
  { "bxor",  0x12 },
  { "bmsk",  0x13 },
  { "add1",  0x14 },
  { "add2",  0x15 },
  { "add3",  0x16 },
  { "sub1",  0x17 },
  { "sub2",  0x18 },
  { "sub3",  0x19 },
  { "mpy",   0x1a },
  { "mpyh",  0x1b },
  { "mpyhu", 0x1c },
  { "mpyu",  0x1d },
  { NULL,    0x00 },
};

struct _table_arc_op table_arc_alu16[] =
{
  //{ "sops",    0x00 },
  { "sub_s",   0x02 },
  { "and_s",   0x04 },
  { "or_s",    0x05 },
  { "bic_s",   0x06 },
  { "xor_s",   0x07 },
  { "tst_s",   0x0b },
  { "mul64_s", 0x0c },
  { "sexb_s",  0x0d },
  { "sexw_s",  0x0e },
  { "extb_s",  0x0f },
  { "extw_s",  0x10 },
  { "abs_s",   0x11 },
  { "not_s",   0x12 },
  { "neg_s",   0x13 },
  { "add1_s",  0x14 },
  { "add2_s",  0x15 },
  { "add3_s",  0x16 },
  { "asl_s",   0x18 },
  { "lsr_s",   0x19 },
  { "asr_s",   0x1a },
  { "asl_s",   0x1b },
  { "asr_s",   0x1c },
  { "lsr_s",   0x1d },
  { "trap_s",  0x1e },
  { "brk_s",   0x1f },
  { NULL,      0x00 },
};

struct _table_arc_op table_arc_single[] =
{
  { "asl",  0x00 },
  { "asr",  0x01 },
  { "lsr",  0x02 },
  { "ror",  0x03 },
  { "rrc",  0x04 },
  { "sexb", 0x05 },
  { "sexw", 0x06 },
  { "extb", 0x07 },
  { "extw", 0x08 },
  { "abs",  0x09 },
  { "not",  0x0a },
  { "rlc",  0x0b },
  { "ex",   0x0c },
  { "zops", 0x3f },
  { NULL,   0x00 },
};

struct _table_arc_op table_arc_none[] =
{
  { "sleep", 0x01 },
  { "swi",   0x02 },
  { "trap0", 0x02 },
  { "sync",  0x03 },
  { "rtie",  0x04 },
  { "brk",   0x05 },
  { NULL,    0x00 },
};

struct _table_arc_op table_arc_special[] =
{
  //{ "jcc",    0x20 },
  //{ "jcc.d",  0x21 },
  //{ "jlcc",   0x22 },
  //{ "jlcc.d", 0x23 },
  //{ "lpcc",   0x28 },
  { "flag",   0x29 },
  { "lr",     0x2A },
  { "sr",     0x2B },
  //{ "sops",   0x2F },
  //{ "ld",     0x30 },
  //{ "ld",     0x31 },
  //{ "ld",     0x32 },
  //{ "ld",     0x33 },
  //{ "ld",     0x34 },
  //{ "ld",     0x35 },
  //{ "ld",     0x36 },
  //{ "ld",     0x37 },
  { NULL,     0x00 },
};

struct _table_arc_op table_arc_alu32[] =
{
  { "asl",    0x00 },
  { "lsr",    0x01 },
  { "asr",    0x02 },
  { "ror",    0x03 },
  { "mul64",  0x04 },
  { "mulu64", 0x05 },
  { "adds",   0x06 },
  { "subs",   0x07 },
  { "divaw",  0x08 },
  { "asls",   0x0a },
  { "asrs",   0x0b },
  { "addsdw", 0x28 },
  { "subsdw", 0x29 },
  //{ "sops",   0x2f },
  { NULL,     0x00 },
};

struct _table_arc_op table_arc_single32[] =
{
  { NULL,    0x00 },
};

struct _table_arc_op table_arc_none32[] =
{
  { NULL,    0x00 },
};

