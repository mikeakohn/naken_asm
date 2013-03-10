#include <stdio.h>
#include <stdlib.h>
#include "table_680x0.h"

//struct _table_680x0 table_680x0[] =

struct _table_680x0_no_operands table_680x0_no_operands[] =
{
  { "illegal", 0x4afc },
  { "nop", 0x4e71 },
  { "rtr", 0x4e77 },
  { "rts", 0x4e75 },
  { "trapv", 0x4e76 },
  { NULL, 0 },
};

struct _table_680x0 table_680x0[] =
{
  { "clr", 0x4200, 0xff00, OP_SINGLE_EA }, // (no immediate extra data)
  { "neg", 0x4400, 0xff00, OP_SINGLE_EA },
  { "negx", 0x4000, 0xff00, OP_SINGLE_EA },
  { "tst", 0x4a00, 0xff00, OP_SINGLE_EA },
  { "jmp", 0x4ec0, 0xffc0, OP_SINGLE_EA_NO_SIZE },
  { "pea", 0x4840, 0xffc0, OP_SINGLE_EA_NO_SIZE },
  { "ori", 0x0000, 0xff00, OP_IMMEDIATE },  // 0
  { "andi", 0x0200, 0xff00, OP_IMMEDIATE }, // 2
  { "subi", 0x0400, 0xff00, OP_IMMEDIATE }, // 4
  { "addi", 0x0600, 0xff00, OP_IMMEDIATE }, // 6
  { "eori", 0x0900, 0xff00, OP_IMMEDIATE }, // 9
  { "cmpi", 0x0c00, 0xff00, OP_IMMEDIATE }, // 12
  { "asl", 0xe1c0, 0xffc0, OP_SHIFT_EA },
  { "asr", 0xe0c0, 0xffc0, OP_SHIFT_EA },
  { "asl", 0xe100, 0xf118, OP_SHIFT },
  { "asr", 0xe000, 0xf118, OP_SHIFT },
  { "lsl", 0xe3c0, 0xffc0, OP_SHIFT_EA },
  { "lsr", 0xe2c0, 0xffc0, OP_SHIFT_EA },
  { "lsl", 0xe108, 0xf118, OP_SHIFT },
  { "lsr", 0xe008, 0xf118, OP_SHIFT },
  { "rol", 0xe7c0, 0xffc0, OP_SHIFT_EA },
  { "ror", 0xe6c0, 0xffc0, OP_SHIFT_EA },
  { "rol", 0xe118, 0xf118, OP_SHIFT },
  { "ror", 0xe018, 0xf118, OP_SHIFT },
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
  "or",    // 1   dir=<ea>+Dn->Dn, Dn+<ea>-><ea>
  "lea",   // 4   dir=1,size=11
  "divs",  // 8   dir=1,size=11
  "divu",  // 8   dir=0,size=11
  "sub",   // 9   dir=<ea>+Dn->Dn, Dn+<ea>-><ea>
  "cmp",   // 11  dir=0
  "eor",   // 11  dir=1
  "muls",  // 12  dir=1,size=11
  "mulu",  // 12  dir=0,size=11
  "and",   // 12  dir=<ea>^Dn->Dn, Dn^<ea>-><ea>
  "add",   // 13  dir=<ea>^Dn->Dn, Dn^<ea>-><ea>
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
NBCD
NOT
PEA
RESET
ROXL
ROXR
RTE
SBCD
Scc
STOP
SUBX
SWAP
TAS
TRAP
UNLK
UNPK
*/

