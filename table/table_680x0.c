#include <stdio.h>
#include <stdlib.h>
#include "table_680x0.h"

struct _table_680x0 table_680x0[] =
{
  { "abcd", 0xc100, 0xf1f0, OP_ABCD },
  { "add", 0xd000, 0xf000, OP_REG_AND_EA },
  { "addi", 0x0600, 0xff00, OP_IMMEDIATE }, // 6
  { "adda", 0xd000, 0xf000, OP_EA_AREG },
  { "addq", 0x5000, 0xf100, OP_QUICK },
  { "addx", 0xd100, 0xf130, OP_EXTENDED },
  { "and", 0xc000, 0xf000, OP_REG_AND_EA },
  { "andi", 0x0200, 0xff00, OP_IMMEDIATE },
  { "asl", 0xe1c0, 0xffc0, OP_SHIFT_EA },
  { "asl", 0xe100, 0xf118, OP_SHIFT },
  { "asr", 0xe0c0, 0xffc0, OP_SHIFT_EA },
  { "asr", 0xe000, 0xf118, OP_SHIFT },
  { "clr", 0x4200, 0xff00, OP_SINGLE_EA }, // (no immediate extra data)
  { "cmp", 0xb000, 0xf000, OP_EA_DREG },
  { "cmpa", 0xb000, 0xf000, OP_EA_AREG },
  { "cmpi", 0x0c00, 0xff00, OP_IMMEDIATE },
  { "cmpm", 0xb108, 0xf138, OP_CMPM },
  { "eor", 0xb000, 0xf000, OP_REG_AND_EA },
  { "eori", 0x0900, 0xff00, OP_IMMEDIATE },
  { "illegal", 0x4afc, 0xffff, OP_NONE },
  { "jmp", 0x4ec0, 0xffc0, OP_SINGLE_EA_NO_SIZE },
  { "jsr", 0x4e80, 0xffc0, OP_SINGLE_EA_TO_ADDR },
  { "lea", 0x41c0, 0xf1c0, OP_LOAD_EA },
  { "lsl", 0xe3c0, 0xffc0, OP_SHIFT_EA },
  { "lsl", 0xe108, 0xf118, OP_SHIFT },
  { "lsr", 0xe2c0, 0xffc0, OP_SHIFT_EA },
  { "lsr", 0xe008, 0xf118, OP_SHIFT },
  { "move", 0x42c0, 0xffc0, OP_MOVE_FROM_CCR },
  { "move", 0x44c0, 0xffc0, OP_MOVE_TO_CCR },
  { "move", 0x40c0, 0xffc0, OP_MOVE_FROM_SR },
  { "movea", 0x0040, 0xc1c0, OP_MOVEA },
  { "moveq", 0x7000, 0xf100, OP_MOVE_QUICK },
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
  { "roxl", 0xe5c0, 0xffc0, OP_ROX_MEM },
  { "roxl", 0xe130, 0xf138, OP_ROX },
  { "roxr", 0xe4c0, 0xffc0, OP_ROX_MEM },
  { "roxr", 0xe110, 0xf138, OP_ROX },
  { "rtm", 0x06c0, 0xfff0, OP_REG },
  { "rtr", 0x4e77, 0xffff, OP_NONE },
  { "rts", 0x4e75, 0xffff, OP_NONE },
  { "sub", 0x9000, 0xf000, OP_REG_AND_EA },
  { "suba", 0x9000, 0xf000, OP_EA_AREG },
  { "subi", 0x0400, 0xff00, OP_IMMEDIATE }, // 4
  { "subq", 0x5100, 0xf100, OP_QUICK },
  { "subx", 0x9100, 0xf130, OP_EXTENDED },
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

// 4 op, 3 reg, 1 dir, 2 size, 6 EA(mode/reg)
table_680x0_alu[]
{
  "divs",  // 8   dir=1,size=11
  "divu",  // 8   dir=0,size=11
  "muls",  // 12  dir=1,size=11
  "mulu",  // 12  dir=0,size=11
};

#endif

/*
Bcc
Bcc.W
BCHG
BCLR
BSET
BSR
BSR.W
BTST
CHK
DBcc
EXG
EXT
LINK
MOVE
MOVE
MOVEM
MOVEP
RTE
SBCD
Scc
STOP
UNPK
*/

