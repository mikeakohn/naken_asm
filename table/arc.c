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
  { "abs_s",  0x7811, 0xf81f, 2, { OP_B,    OP_C,    OP_NONE } },
  { "add_s",  0x6018, 0xf818, 3, { OP_A,    OP_B,    OP_C    } },
  { "add_s",  0x6800, 0xf818, 3, { OP_C,    OP_B,    OP_U3   } },
  { "add_s",  0x7000, 0xf818, 3, { OP_B,    OP_B,    OP_H    } },
  { "add_s",  0xc080, 0xf8e0, 3, { OP_B,    OP_SP,   OP_U7   } },
  { "add_s",  0xc0a0, 0xffe0, 3, { OP_SP,   OP_SP,   OP_U7   } },
  { "add_s",  0xce00, 0xfe00, 3, { OP_R0,   OP_GP,   OP_S11  } },
  { "add_s",  0xe000, 0xf880, 3, { OP_B,    OP_B,    OP_U7   } },
  { "add_s",  0x70c7, 0xf8ff, 3, { OP_B,    OP_B,    OP_LIMM } },
  { "add1_s", 0x7814, 0xf81f, 3, { OP_B,    OP_B,    OP_C    } },
  { "add2_s", 0x7815, 0xf81f, 3, { OP_B,    OP_B,    OP_C    } },
  { "add3_s", 0x7816, 0xf81f, 3, { OP_B,    OP_B,    OP_C    } },
  { "and_s",  0x7804, 0xf81f, 3, { OP_B,    OP_B,    OP_C    } },
  { "asl_s",  0x781b, 0xf81f, 2, { OP_B,    OP_C,    OP_NONE } },
  { "asl_s",  0x6810, 0xf818, 3, { OP_C,    OP_B,    OP_U3   } },
  { "asl_s",  0x7818, 0xf81f, 3, { OP_B,    OP_B,    OP_C    } },
  { "asl_s",  0xb800, 0xf8e0, 3, { OP_B,    OP_B,    OP_U5   } },
  { "asr_s",  0x781c, 0xf81f, 2, { OP_B,    OP_C,    OP_NONE } },
  { "asr_s",  0x6818, 0xf818, 3, { OP_C,    OP_B,    OP_U3   } },
  { "asr_s",  0x781a, 0xf81f, 3, { OP_B,    OP_B,    OP_C    } },
  { "asr_s",  0xb840, 0xf8e0, 3, { OP_B,    OP_B,    OP_U5   } },
  { "bclr_s", 0xb8a0, 0xf8e0, 3, { OP_B,    OP_B,    OP_U5   } },
  { "bic_s",  0x7806, 0xf81f, 3, { OP_B,    OP_B,    OP_C    } },
  { "bmsk_s", 0xb8c0, 0xf8e0, 3, { OP_B,    OP_B,    OP_U5   } },
  { "brk_s",  0x7fff, 0xffff, 0, { OP_NONE, OP_NONE, OP_NONE } },
  { "bset_s", 0xb880, 0xf8e0, 3, { OP_B,    OP_B,    OP_U5   } },
  { "btst_s", 0xb880, 0xb8e0, 2, { OP_B,    OP_U5,   OP_NONE } },
  { "cmp_s",  0x7010, 0xf818, 2, { OP_B,    OP_H,    OP_NONE } },
  { "cmp_s",  0xe080, 0xf880, 2, { OP_B,    OP_U7,   OP_NONE } },
  { "cmp_s",  0x70d7, 0xf8ff, 2, { OP_B,    OP_LIMM, OP_NONE } },
  { "extb_s", 0x780f, 0xf81f, 2, { OP_B,    OP_C,    OP_NONE } },
  { "extw_s", 0x7810, 0xf81f, 2, { OP_B,    OP_C,    OP_NONE } },
  { "lsr_s",  0x781d, 0xf81f, 2, { OP_B,    OP_C,    OP_NONE } },
  { "lsr_s",  0x7819, 0xf81f, 3, { OP_B,    OP_B,    OP_C    } },
  { "lsr_s",  0xb820, 0xf8e0, 3, { OP_B,    OP_B,    OP_U5   } },
  { "mov_s",  0x7008, 0xf818, 2, { OP_B,    OP_H,    OP_NONE } },
  { "mov_s",  0x7018, 0xf818, 2, { OP_H,    OP_H,    OP_NONE } },
  { "mov_s",  0xd800, 0xf800, 2, { OP_B,    OP_U8,   OP_NONE } },
  { "mov_s",  0x70cf, 0xf8ff, 2, { OP_B,    OP_LIMM, OP_NONE } },
  { "neg_s",  0x7813, 0xf81f, 2, { OP_B,    OP_C,    OP_NONE } },
  { "nop_s",  0x78e0, 0xffff, 0, { OP_NONE, OP_NONE, OP_NONE } },
  { "not_s",  0x7812, 0xf81f, 2, { OP_B,    OP_C,    OP_NONE } },
  { "or_s",   0x7805, 0xf81f, 3, { OP_B,    OP_B,    OP_C    } },
  { "sexw_s", 0x780e, 0xf81f, 2, { OP_B,    OP_C,    OP_NONE } },
  { "sub_s",  0x6808, 0xf818, 3, { OP_C,    OP_B,    OP_U3   } },
  { "sub_s",  0x6802, 0xf81f, 3, { OP_B,    OP_B,    OP_C    } },
  { "sub_s",  0xb860, 0xf8e0, 3, { OP_B,    OP_B,    OP_U5   } },
  //{ "sub_s.ne", 0x78c0, 0xf8ff, 3, { OP_B,   OP_B,    OP_B    } },
  //{ "sub_s",   0xc1a0, 0xffe0, 3, { OP_SP,   OP_SP,   OP_U7   } },
  //{ "trap_s",  0x788e, 0xf88f, 2, { OP_U6,   OP_NONE, OP_NONE } },
  { "tst_s",  0x780b, 0xf81f, 2, { OP_B,    OP_C,    OP_NONE } },
  { "unimp_s",0x79e0, 0xffff, 0, { OP_NONE, OP_NONE, OP_NONE } },
  { "xor_s",  0x7807, 0xf81f, 3, { OP_B,    OP_B,    OP_C    } },

#if 0
  { "pop_s",  0xc0c1, 0xf8ff, 1, OP_B },
  { "pop_s",  0xc0d1, 0xf8ff, 1, OP_BLINK },
  { "push_s", 0xc0e1, 0xf8ff, 1, OP_B },
  { "push_s", 0xc0e1, 0xf8ff, 1, OP_BLINK },

  // FIXME: Change the format so all the operands are seprated and probably
  //        combine with 32 bit instructions.
#endif

  { NULL, 0, 0, 0 },
};

struct _table_arc_op table_arc_alu[] =
{
  { "add",    0x00, 0 },
  { "adc",    0x01, 0 },
  { "sub",    0x02, 0 },
  { "sbc",    0x03, 0 },
  { "and",    0x04, 0 },
  { "or",     0x05, 0 },
  { "bic",    0x06, 0 },
  { "xor",    0x07, 0 },
  { "max",    0x08, 0 },
  { "min",    0x09, 0 },
  { "mov",    0x0a, 0 },
  { "tst",    0x0b, 0 },
  { "cmp",    0x0c, 0 },
  { "rcmp",   0x0d, 0 },
  { "rsub",   0x0e, 0 },
  { "bset",   0x0f, 0 },
  { "bclr",   0x10, 0 },
  { "btst",   0x11, 0 },
  { "bxor",   0x12, 0 },
  { "bmsk",   0x13, 0 },
  { "add1",   0x14, 0 },
  { "add2",   0x15, 0 },
  { "add3",   0x16, 0 },
  { "sub1",   0x17, 0 },
  { "sub2",   0x18, 0 },
  { "sub3",   0x19, 0 },
  { "mpy",    0x1a, 0 },
  { "mpyh",   0x1b, 0 },
  { "mpyhu",  0x1c, 0 },
  { "mpyu",   0x1d, 0 },
  { "asl",    0x00, 1 },
  { "lsr",    0x01, 1 },
  { "asr",    0x02, 1 },
  { "ror",    0x03, 1 },
  { "mul64",  0x04, 1 },
  { "mulu64", 0x05, 1 },
  { "adds",   0x06, 1 },
  { "subs",   0x07, 1 },
  { "divaw",  0x08, 1 },
  { "asls",   0x0a, 1 },
  { "asrs",   0x0b, 1 },
  { "addsdw", 0x28, 1 },
  { "subsdw", 0x29, 1 },
  { NULL,     0x00, 0 },
};

struct _table_arc_op table_arc_alu16[] =
{
  //{ "sops",    0x00, 0 },
  { "sub_s",   0x02, 0 },
  { "and_s",   0x04, 0 },
  { "or_s",    0x05, 0 },
  { "bic_s",   0x06, 0 },
  { "xor_s",   0x07, 0 },
  { "tst_s",   0x0b, 0 },
  { "mul64_s", 0x0c, 0 },
  { "sexb_s",  0x0d, 0 },
  { "sexw_s",  0x0e, 0 },
  { "extb_s",  0x0f, 0 },
  { "extw_s",  0x10, 0 },
  { "abs_s",   0x11, 0 },
  { "not_s",   0x12, 0 },
  { "neg_s",   0x13, 0 },
  { "add1_s",  0x14, 0 },
  { "add2_s",  0x15, 0 },
  { "add3_s",  0x16, 0 },
  { "asl_s",   0x18, 0 },
  { "lsr_s",   0x19, 0 },
  { "asr_s",   0x1a, 0 },
  { "asl_s",   0x1b, 0 },
  { "asr_s",   0x1c, 0 },
  { "lsr_s",   0x1d, 0 },
  { "trap_s",  0x1e, 0 },
  { "brk_s",   0x1f, 0 },
  { NULL,      0x00, 0 },
};

struct _table_arc_op table_arc_single[] =
{
  { "asl",  0x00, 0 },
  { "asr",  0x01, 0 },
  { "lsr",  0x02, 0 },
  { "ror",  0x03, 0 },
  { "rrc",  0x04, 0 },
  { "sexb", 0x05, 0 },
  { "sexw", 0x06, 0 },
  { "extb", 0x07, 0 },
  { "extw", 0x08, 0 },
  { "abs",  0x09, 0 },
  { "not",  0x0a, 0 },
  { "rlc",  0x0b, 0 },
  { "ex",   0x0c, 0 },
  { "zops", 0x3f, 0 },
  { NULL,   0x00, 0 },
};

struct _table_arc_op table_arc_none[] =
{
  { "sleep", 0x01, 0 },
  { "swi",   0x02, 0 },
  { "trap0", 0x02, 0 },
  { "sync",  0x03, 0 },
  { "rtie",  0x04, 0 },
  { "brk",   0x05, 0 },
  { NULL,    0x00, 0 },
};

struct _table_arc_op table_arc_special[] =
{
  //{ "jcc",    0x20, 0 },
  //{ "jcc.d",  0x21, 0 },
  //{ "jlcc",   0x22, 0 },
  //{ "jlcc.d", 0x23, 0 },
  //{ "lpcc",   0x28, 0 },
  { "flag",   0x29, 0 },
  { "lr",     0x2a, 0 },
  { "sr",     0x2b, 0 },
  //{ "sops",   0x2f, 0 },
  //{ "ld",     0x30, 0 },
  //{ "ld",     0x31, 0 },
  //{ "ld",     0x32, 0 },
  //{ "ld",     0x33, 0 },
  //{ "ld",     0x34, 0 },
  //{ "ld",     0x35, 0 },
  //{ "ld",     0x36, 0 },
  //{ "ld",     0x37, 0 },
  { NULL,     0x00, 0 },
};

struct _table_arc table_arc_load_store[] =
{
  // ld<zz><.x><.aa><di> a, [b, s9]
  // ld<zz><.x><di>      a, [limm]       (s = 0, b = limm, aa = 0)
  { "ld",     0x10000000, 0xf8000180, OP_A_PAREN_B_S9, FLAG_ZERO },
  { "ldb",    0x10000080, 0xf8000180, OP_A_PAREN_B_S9, FLAG_ZERO },
  { "ldw",    0x10000100, 0xf8000180, OP_A_PAREN_B_S9, FLAG_ZERO },
  //{ "ld",     0x16007000, 0xfffff780, OP_A_PAREN_LIMM, FLAG_ZERO },
  //{ "ldb",    0x10007080, 0xfffff780, OP_A_PAREN_LIMM, FLAG_ZERO },
  //{ "ldw",    0x10007100, 0xfffff780, OP_A_PAREN_LIMM, FLAG_ZERO },

  // ld<zz><.x><.aa><di> a, [b, c]
  // ld<zz><.x><.aa><di> a, [b, limm]    (c = limm)
  // ld<zz><.x><di>      a, [limm, c]    (b = limm)
  { "ld",     0x203e0000, 0xf8000000, OP_A_PAREN_B_C,  FLAG_ZERO },
  { "ldb",    0x203e0080, 0xf8000000, OP_A_PAREN_B_C,  FLAG_ZERO },
  { "ldw",    0x203e0100, 0xf8000000, OP_A_PAREN_B_C,  FLAG_ZERO },

  // st<zz><.aa><.di>    c, [b, s9]
  // st<zz><.di>         c, [limm]       (s = 0, b = limm, aa = 0)
  // st<zz><.aa><.di>    limm, [b, s9]   (c = limm)
  { "st",     0x18000000, 0xf8000007, OP_C_PAREN_B_S9, FLAG_NONE },
  { "stb",    0x18000002, 0xf8000007, OP_C_PAREN_B_S9, FLAG_NONE },
  { "stw",    0x18000004, 0xf8000007, OP_C_PAREN_B_S9, FLAG_NONE },

  { NULL,     0x00000000, 0x00000000, 0,               0 }
};

struct _table_arc table_arc_load_store16[] =
{
  { "ld_s",   0x6000, 0xf818, OP_A_PAREN_B_C     },
  { "ldb_s",  0x6008, 0xf818, OP_A_PAREN_B_C     },
  { "ldw_s",  0x6010, 0xf818, OP_A_PAREN_B_C     },
  { "ld_s",   0x8000, 0xf800, OP_C_PAREN_B_U7    },
  { "ldb_s",  0x8800, 0xf800, OP_C_PAREN_B_U5    },
  { "ldw_s",  0x9000, 0xf800, OP_C_PAREN_B_U6    },
  { "ldw_s.x",0x9800, 0xf800, OP_C_PAREN_B_U6    },
  { "ld_s",   0xc000, 0xf8e0, OP_B_PAREN_SP_U7   },
  { "ldb_s",  0xc020, 0xf8e0, OP_B_PAREN_SP_U7   },
  { "ld_s",   0xc800, 0xfe00, OP_R0_PAREN_GP_S11 },
  { "ldb_s",  0xca00, 0xfe00, OP_R0_PAREN_GP_S9  },
  { "ldw_s",  0xcc00, 0xfe00, OP_R0_PAREN_GP_S10 },
  { "ld_s",   0xd000, 0xf800, OP_B_PAREN_PCL_U10 },

  { "st_s",   0xa000, 0xf800, OP_C_PAREN_B_U7    },
  { "stb_s",  0xa800, 0xf800, OP_C_PAREN_B_U5    },
  { "stw_s",  0xb000, 0xf800, OP_C_PAREN_B_U6    },
  { "st_s",   0xc040, 0xf8e0, OP_B_PAREN_SP_U7   },
  { "stb_s",  0xc060, 0xf8e0, OP_B_PAREN_SP_U7   },
  { NULL,     0x0000, 0x0000, 0                  },
};

#if 0
struct _table_arc_op table_arc_single32[] =
{
  { NULL,    0x00, 0 },
};

struct _table_arc_op table_arc_none32[] =
{
  { NULL,    0x00, 0 },
};
#endif

