#include <stdio.h>
#include <stdlib.h>
#include "table_680x0.h"

//struct _table_680x0 table_680x0[] =

#if 0
struct _table_680x0_no_operands table_680x0_no_operands[] =
{
  { "illegal", 0x4afc },
  { "reset", 0x4e70 },
  { "nop", 0x4e71 },
  { "rtr", 0x4e77 },
  { "rts", 0x4e75 },
  { "trapv", 0x4e76 },
  { NULL, 0 },
};
#endif

struct _table_680x0 table_680x0[] =
{
  { "add", 0xd000, 0xf000, OP_REG_AND_EA },
  { "addi", 0x0600, 0xff00, OP_IMMEDIATE }, // 6
  { "adda", 0xd000, 0xf000, OP_EA_AREG },
  { "and", 0xc000, 0xf000, OP_REG_AND_EA },
  { "andi", 0x0200, 0xff00, OP_IMMEDIATE }, // 2
  { "asl", 0xe1c0, 0xffc0, OP_SHIFT_EA },
  { "asl", 0xe100, 0xf118, OP_SHIFT },
  { "asr", 0xe0c0, 0xffc0, OP_SHIFT_EA },
  { "asr", 0xe000, 0xf118, OP_SHIFT },
  { "clr", 0x4200, 0xff00, OP_SINGLE_EA }, // (no immediate extra data)
  { "cmpi", 0x0c00, 0xff00, OP_IMMEDIATE }, // 12
  { "eor", 0xb000, 0xf000, OP_REG_AND_EA },
  { "eori", 0x0900, 0xff00, OP_IMMEDIATE }, // 9
  { "illegal", 0x4afc, 0xffff, OP_NONE },
  { "jmp", 0x4ec0, 0xffc0, OP_SINGLE_EA_NO_SIZE },
  { "lsl", 0xe3c0, 0xffc0, OP_SHIFT_EA },
  { "lsl", 0xe108, 0xf118, OP_SHIFT },
  { "lsr", 0xe2c0, 0xffc0, OP_SHIFT_EA },
  { "lsr", 0xe008, 0xf118, OP_SHIFT },
  { "nbcd", 0x4800, 0xff00, OP_SINGLE_EA_NO_SIZE },
  { "neg", 0x4400, 0xff00, OP_SINGLE_EA },
  { "negx", 0x4000, 0xff00, OP_SINGLE_EA },
  { "nop", 0x4e71, 0xffff, OP_NONE },
  { "not", 0x4600, 0xff00, OP_SINGLE_EA },
  { "or", 0x8000, 0xf000, OP_REG_AND_EA },
  { "ori", 0x0000, 0xff00, OP_IMMEDIATE },  // 0
  { "pea", 0x4840, 0xffc0, OP_SINGLE_EA_NO_SIZE },
  { "reset", 0x4e70, 0xffff, OP_NONE },
  { "rol", 0xe7c0, 0xffc0, OP_SHIFT_EA },
  { "rol", 0xe118, 0xf118, OP_SHIFT },
  { "ror", 0xe6c0, 0xffc0, OP_SHIFT_EA },
  { "ror", 0xe018, 0xf118, OP_SHIFT },
  { "rtm", 0x06c0, 0xfff0, OP_REG },
  { "rtr", 0x4e77, 0xffff, OP_NONE },
  { "rts", 0x4e75, 0xffff, OP_NONE },
  { "sub", 0x9000, 0xf000, OP_REG_AND_EA },
  { "subi", 0x0400, 0xff00, OP_IMMEDIATE }, // 4
  { "swap", 0x4840, 0xfff8, OP_AREG },
  { "tas", 0x4ac0, 0xffc0, OP_SINGLE_EA_NO_SIZE },
  { "tst", 0x4a00, 0xff00, OP_SINGLE_EA },
  { "trapv", 0x4e76, 0xffff, OP_NONE },
  { "trap", 0x4e40, 0xfff0, OP_VECTOR },
  { "unlk", 0x4e58, 0xfff8, OP_AREG },
  { NULL, 0x0000, 0x0000, OP_NONE },
};

char *table_680x0_condition_codes[] =
{
  "t",
  "f",
  "hi",
  "ls",
  "cc",  // hi
  "cs",  // lo
  "ne",
  "eq",
  "vc",
  "vs",
  "pl",
  "mi",
  "ge",
  "lt",
  "gt",
  "le",
};

#if 0

struct _table_680x0_quick *table_680x0_quick[]
{
  { "ADDQ", 0x5 0 },
  { "MOVEQ", 0x7, 2 },  // different format than ADDQ or SUBQ
  { "SUBQ", 0x5, 1 },
  { NULL, 0 }
};

// 4 op, 3 reg, 1 dir, 2 size, 6 EA(mode/reg)
table_680x0_alu[]
{
  "lea",   // 4   dir=1,size=11
  "divs",  // 8   dir=1,size=11
  "divu",  // 8   dir=0,size=11
  "cmp",   // 11  dir=0
  "muls",  // 12  dir=1,size=11
  "mulu",  // 12  dir=0,size=11
};

// 4 op, 3 reg, 3 mode, 6 EA(mode/reg)
table_680x0_alua[]
{
  "SUBA",  // 9
  "CMPA",  // 11  mode=011:word,111:long
  "ADDA",  // 13  mode=011:word,111:long
};
#endif

/*
ABCD
ADDX
Bcc
Bcc.W
BCHG
BCLR
BSET
BSR
BSR.W
BTST
CHK
CMPM
DBcc
EXG
EXT
JSR
LINK
MOVE
MOVE
MOVE
MOVE
MOVE
MOVEA
MOVEM
MOVEP
ROXL
ROXR
RTE
SBCD
Scc
STOP
SUBX
SWAP
UNPK
*/

