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

#if 0
table_680x0_condition_codes =
{
  "T",
  "F",
  "HI",
  "LS",
  "CC",  // HI
  "CS",  // LO
  "NE",
  "EQ",
  "VC",
  "VS",
  "PL",
  "MI",
  "GE",
  "LT",
  "GT",
  "LE",
};



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
  "OR",    // 1   dir=<ea>+Dn->Dn, Dn+<ea>-><ea>
  "LEA",   // 4   dir=1,size=11
  "DIVS",  // 8   dir=1,size=11
  "DIVU",  // 8   dir=0,size=11
  "SUB",   // 9   dir=<ea>+Dn->Dn, Dn+<ea>-><ea>
  "CMP",   // 11  dir=0
  "EOR",   // 11  dir=1
  "MULS",  // 12  dir=1,size=11
  "MULU",  // 12  dir=0,size=11
  "AND",   // 12  dir=<ea>^Dn->Dn, Dn^<ea>-><ea>
  "ADD",   // 13  dir=<ea>^Dn->Dn, Dn^<ea>-><ea>
  "ASR",   // 14  dir=0 size=11  REVIEW (this is memory only)
  "ASL",   // 14  dir=1 size=11  REVIEW
  "LSR",   // 14  dir=0
  "LSL",   // 14  dir=1
  "ROR",   // 15  dir=0 size=11  REVIEW
  "ROL",   // 15  dir=1 size=11  REVIEW
};

// 8 op, 2 size, 6 EA(mode/reg)  [16/8] [32]
table_680x0_alui[]
{
  "ORI",   // 0
  "ANDI",  // 2
  "SUBI",  // 4
  "ADDI",  // 6
  "EORI",  // 9
  "CMPI",  // 12
  "CLR",   // 0x42  (no immediate extra data)
  "NEG",   // 0x44  (no immediate extra data)
  "TST",   // 0x4a  (no immediate extra data)
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
JMP
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
NEGX
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

