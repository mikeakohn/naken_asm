#include <stdio.h>
#include <stdlib.h>
#include "table_680x0.h"

struct _table_680x0 table_680x0[] =
{
  { "abcd", 0xc100, 0xf1f0, OP_BCD },
  { "add", 0xd000, 0xf000, OP_REG_AND_EA },
  { "addi", 0x0600, 0xff00, OP_IMMEDIATE },
  { "adda", 0xd000, 0xf000, OP_EA_AREG },
  { "addq", 0x5000, 0xf100, OP_QUICK },
  { "addx", 0xd100, 0xf130, OP_EXTENDED },
  { "and", 0xc000, 0xf000, OP_REG_AND_EA },
  { "andi", 0x0200, 0xff00, OP_IMMEDIATE },
  { "andi", 0x023c, 0xffff, OP_LOGIC_CCR },
  { "asl", 0xe1c0, 0xffc0, OP_SHIFT_EA },
  { "asl", 0xe100, 0xf118, OP_SHIFT },
  { "asr", 0xe0c0, 0xffc0, OP_SHIFT_EA },
  { "asr", 0xe000, 0xf118, OP_SHIFT },
  { "bchg", 0x0140, 0xf1c0, OP_REG_EA_NO_SIZE },
  { "bchg", 0x0840, 0xffc0, OP_EXTRA_IMM_EA },
  { "bclr", 0x0180, 0xf1c0, OP_REG_EA_NO_SIZE },
  { "bclr", 0x0880, 0xffc0, OP_EXTRA_IMM_EA },
  { "bkpt", 0x4848, 0xfff8, OP_VECTOR3 },
  { "bset", 0x01c0, 0xf1c0, OP_REG_EA_NO_SIZE },
  { "bset", 0x08c0, 0xffc0, OP_EXTRA_IMM_EA },
  { "bsr", 0x6100, 0xff00, OP_DISPLACEMENT },
  { "bra", 0x6000, 0xff00, OP_DISPLACEMENT },
  { "btst", 0x0100, 0xf1c0, OP_REG_EA_NO_SIZE },
  { "btst", 0x0800, 0xffc0, OP_EXTRA_IMM_EA },
  { "chk", 0x4000, 0xf040, OP_EA_DREG_WL },
  { "clr", 0x4200, 0xff00, OP_SINGLE_EA }, // (no immediate extra data)
  { "cmp", 0xb000, 0xf000, OP_EA_DREG },
  { "cmpa", 0xb000, 0xf000, OP_EA_AREG },
  { "cmpi", 0x0c00, 0xff00, OP_IMMEDIATE },
  { "cmpm", 0xb108, 0xf138, OP_CMPM },
  { "divs", 0x81c0, 0xf1c0, OP_DIV_MUL },
  { "divu", 0x80c0, 0xf1c0, OP_DIV_MUL },
  { "eor", 0xb000, 0xf000, OP_REG_AND_EA },
  { "eori", 0x0900, 0xff00, OP_IMMEDIATE },
  { "eori", 0x0a3c, 0xffff, OP_LOGIC_CCR },
  { "exg", 0xc100, 0xf100, OP_EXCHANGE },
  { "ext", 0x4800, 0xfe38, OP_EXT },
  { "illegal", 0x4afc, 0xffff, OP_NONE },
  { "jmp", 0x4ec0, 0xffc0, OP_SINGLE_EA_NO_SIZE },
  { "jsr", 0x4e80, 0xffc0, OP_SINGLE_EA_TO_ADDR },
  { "lea", 0x41c0, 0xf1c0, OP_LOAD_EA },
  { "link", 0x4e50, 0xfff8, OP_LINK },
  { "lsl", 0xe3c0, 0xffc0, OP_SHIFT_EA },
  { "lsl", 0xe108, 0xf118, OP_SHIFT },
  { "lsr", 0xe2c0, 0xffc0, OP_SHIFT_EA },
  { "lsr", 0xe008, 0xf118, OP_SHIFT },
  { "move", 0x0000, 0xc000, OP_MOVE },
  { "move", 0x42c0, 0xffc0, OP_MOVE_FROM_CCR },
  { "move", 0x44c0, 0xffc0, OP_MOVE_TO_CCR },
  { "move", 0x40c0, 0xffc0, OP_MOVE_FROM_SR },
  { "movea", 0x0040, 0xc1c0, OP_MOVEA },
  { "moveq", 0x7000, 0xf100, OP_MOVE_QUICK },
  { "movep", 0x0008, 0xf038, OP_MOVEP },
  { "movem", 0x4880, 0xfb80, OP_MOVEM },
  { "muls", 0xc1c0, 0xf1c0, OP_DIV_MUL },
  { "mulu", 0xc1c0, 0xf1c0, OP_DIV_MUL },
  { "nbcd", 0x4800, 0xffc0, OP_SINGLE_EA_NO_SIZE },
  { "neg", 0x4400, 0xff00, OP_SINGLE_EA },
  { "negx", 0x4000, 0xff00, OP_SINGLE_EA },
  { "nop", 0x4e71, 0xffff, OP_NONE },
  { "not", 0x4600, 0xff00, OP_SINGLE_EA },
  { "or", 0x8000, 0xf000, OP_REG_AND_EA },
  { "ori", 0x0000, 0xff00, OP_IMMEDIATE },
  { "ori", 0x003c, 0xffff, OP_LOGIC_CCR },
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
  { "sbcd", 0x8100, 0xf1f0, OP_BCD },
  { "sub", 0x9000, 0xf000, OP_REG_AND_EA },
  { "suba", 0x9000, 0xf000, OP_EA_AREG },
  { "subi", 0x0400, 0xff00, OP_IMMEDIATE },
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

/*
MOVE
MOVE
MOVEM
MOVEP

EXTB
RTE
STOP
UNPK
*/

