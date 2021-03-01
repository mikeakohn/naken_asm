/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2021 by Michael Kohn
 *
 */

#include "table/68000.h"

struct _table_68000 table_68000[] =
{
  { "abcd", 0xc100, 0xf1f0, OP_BCD, 0, 0, OMIT_W|OMIT_L, DEFAULT_B },
  { "add", 0xd000, 0xf000, OP_REG_AND_EA, 0, MODE_DN|MODE_AN|MODE_D16_PC|MODE_D8_PC_XN, 0, 0 },
  { "adda", 0xd000, 0xf000, OP_EA_AREG, 0, 0, OMIT_B, 0 },
  { "addi", 0x0600, 0xff00, OP_IMMEDIATE, 0, MODE_AN|MODE_D16_PC|MODE_D8_PC_XN, 0, 0 },
  { "addq", 0x5000, 0xf100, OP_QUICK, 0, MODE_D16_PC|MODE_D8_PC_XN, 0, 0 },
  { "addx", 0xd100, 0xf130, OP_EXTENDED, 0, 0, 0, 0 },
  { "and", 0xc000, 0xf000, OP_REG_AND_EA, MODE_AN, MODE_DN|MODE_AN|MODE_D16_PC|MODE_D8_PC_XN, 0, 0 },
  { "andi", 0x0200, 0xff00, OP_IMMEDIATE, 0, MODE_AN|MODE_D16_PC|MODE_D8_PC_XN, 0, 0 },
  { "andi", 0x023c, 0xffff, OP_LOGIC_CCR, 0, 0, OMIT_W|OMIT_L, DEFAULT_B },
  { "andi", 0x027c, 0xffff, OP_LOGIC_SR, 0, 0, OMIT_W|OMIT_L, DEFAULT_W },
  { "asl", 0xe1c0, 0xffc0, OP_SHIFT_EA, 0, MODE_DN|MODE_AN|MODE_D16_PC|MODE_D8_PC_XN, OMIT_B|OMIT_L, DEFAULT_W },
  { "asl", 0xe100, 0xf118, OP_SHIFT, 0, 0, 0, 0 },
  { "asr", 0xe0c0, 0xffc0, OP_SHIFT_EA, 0, MODE_DN|MODE_AN|MODE_D16_PC|MODE_D8_PC_XN, OMIT_B|OMIT_L, DEFAULT_W },
  { "asr", 0xe000, 0xf118, OP_SHIFT, 0, 0, 0, 0 },
  { "bchg", 0x0140, 0xf1c0, OP_BIT_REG_EA, 0, MODE_AN|MODE_D16_PC|MODE_D8_PC_XN, NO_SIZE, 0 },
  { "bchg", 0x0840, 0xffc0, OP_BIT_IMM_EA, 0, MODE_AN|MODE_D16_PC|MODE_D8_PC_XN, NO_SIZE, 0 },
  { "bclr", 0x0180, 0xf1c0, OP_BIT_REG_EA, 0, MODE_AN|MODE_D16_PC|MODE_D8_PC_XN, NO_SIZE, 0 },
  { "bclr", 0x0880, 0xffc0, OP_BIT_IMM_EA, 0, MODE_AN|MODE_D16_PC|MODE_D8_PC_XN, NO_SIZE, 0 },
  { "bkpt", 0x4848, 0xfff8, OP_VECTOR3, 0, 0, NO_SIZE, 0 },
  { "bsr", 0x6100, 0xff00, OP_BRANCH, 0, 0, 0, 0 },
  { "bra", 0x6000, 0xff00, OP_BRANCH, 0, 0, 0, 0 },
  { "bset", 0x01c0, 0xf1c0, OP_BIT_REG_EA, 0, MODE_AN|MODE_D16_PC|MODE_D8_PC_XN, NO_SIZE, 0 },
  { "bset", 0x08c0, 0xffc0, OP_BIT_IMM_EA, 0, MODE_AN|MODE_D16_PC|MODE_D8_PC_XN, NO_SIZE, 0 },
  { "btst", 0x0100, 0xf1c0, OP_BIT_REG_EA, 0, MODE_AN, NO_SIZE, 0 },
  { "btst", 0x0800, 0xffc0, OP_BIT_IMM_EA, 0, MODE_AN, NO_SIZE, 0 },
  { "chk", 0x4000, 0xf040, OP_EA_DREG_WL, MODE_AN, 0, OMIT_B, 0 },
  { "clr", 0x4200, 0xff00, OP_SINGLE_EA, 0, MODE_AN|MODE_D16_PC|MODE_D8_PC_XN, 0, 0 },
  { "cmp", 0xb000, 0xf000, OP_EA_DREG, 0, 0, 0, 0 },
  { "cmpa", 0xb000, 0xf000, OP_EA_AREG, 0, 0, OMIT_B, 0 },
  { "cmpi", 0x0c00, 0xff00, OP_IMMEDIATE, 0, MODE_AN, 0, 0 },
  { "cmpm", 0xb108, 0xf138, OP_CMPM, 0, 0, 0, 0 },
  { "divs", 0x81c0, 0xf1c0, OP_DIV_MUL, MODE_AN, 0, OMIT_B, 0 },
  { "divu", 0x80c0, 0xf1c0, OP_DIV_MUL, MODE_AN, 0, OMIT_B, 0 },
  { "eor", 0xb000, 0xf000, OP_DREG_EA, 0, MODE_AN|MODE_D16_PC|MODE_D8_PC_XN, 0, 0 },
  { "eori", 0x0a00, 0xff00, OP_IMMEDIATE, 0, MODE_AN|MODE_D16_PC|MODE_D8_PC_XN, 0, 0 },
  { "eori", 0x0a3c, 0xffff, OP_LOGIC_CCR, 0, 0, OMIT_W|OMIT_L, DEFAULT_B },
  { "eori", 0x0a7c, 0xffff, OP_LOGIC_SR, 0, 0, OMIT_B|OMIT_L, DEFAULT_W },
  { "exg", 0xc100, 0xf100, OP_EXCHANGE, 0, 0, OMIT_B|OMIT_W, DEFAULT_L },
  { "ext", 0x4800, 0xfe38, OP_EXT, 0, 0, OMIT_B, 0 },
  { "illegal", 0x4afc, 0xffff, OP_NONE, 0, 0, NO_SIZE, 0 },
  { "jmp", 0x4ec0, 0xffc0, OP_JUMP, 0, MODE_DN|MODE_AN|MODE_AN_P|MODE_AN_N, NO_SIZE, 0 },
  { "jsr", 0x4e80, 0xffc0, OP_JUMP, 0, MODE_DN|MODE_AN|MODE_AN_P|MODE_AN_N, NO_SIZE, 0 },
  { "lea", 0x41c0, 0xf1c0, OP_LOAD_EA, MODE_DN|MODE_AN|MODE_AN_P|MODE_AN_N|MODE_IMM, 0, OMIT_B|OMIT_W, DEFAULT_L },
  { "link", 0x4e50, 0xfff8, OP_LINK_W, 0, 0, OMIT_B|OMIT_L, DEFAULT_W },
  { "link", 0x4808, 0xfff8, OP_LINK_L, 0, 0, OMIT_B|OMIT_W, DEFAULT_L },
  { "lsl", 0xe3c0, 0xffc0, OP_SHIFT_EA, 0, MODE_DN|MODE_AN|MODE_D16_PC|MODE_D8_PC_XN, OMIT_B|OMIT_L, DEFAULT_W },
  { "lsl", 0xe108, 0xf118, OP_SHIFT, 0, 0, 0 },
  { "lsr", 0xe2c0, 0xffc0, OP_SHIFT_EA, 0, MODE_DN|MODE_AN|MODE_D16_PC|MODE_D8_PC_XN, OMIT_B|OMIT_L, DEFAULT_W },
  { "lsr", 0xe008, 0xf118, OP_SHIFT, 0, 0, 0, 0 },
  { "move", 0x0000, 0xc000, OP_MOVE, 0, MODE_AN|MODE_D16_PC|MODE_D8_PC_XN, 0, 0 },
  { "move", 0x42c0, 0xffc0, OP_MOVE_FROM_CCR, 0, MODE_AN|MODE_D16_PC|MODE_D8_PC_XN, OMIT_B|OMIT_L, DEFAULT_W },
  { "move", 0x44c0, 0xffc0, OP_MOVE_TO_CCR, MODE_AN, 0, OMIT_B|OMIT_L, DEFAULT_W },
  { "move", 0x40c0, 0xffc0, OP_MOVE_FROM_SR, 0, MODE_AN|MODE_D16_PC|MODE_D8_PC_XN, OMIT_B|OMIT_L, DEFAULT_W },
  { "move", 0x46c0, 0xffc0, OP_MOVE_TO_SR, MODE_AN, 0, OMIT_B|OMIT_L, DEFAULT_W },
  { "movea", 0x0040, 0xc1c0, OP_MOVEA, 0, 0, OMIT_B, 0 },
  { "movem", 0x4880, 0xfb80, OP_MOVEM, MODE_DN|MODE_AN|MODE_AN_N|MODE_IMM, MODE_DN|MODE_AN|MODE_AN_P|MODE_IMM|MODE_D16_PC|MODE_D8_PC_XN, OMIT_B, DEFAULT_W },
  { "movep", 0x0008, 0xf038, OP_MOVEP, 0, 0, OMIT_B, DEFAULT_W },
  { "moveq", 0x7000, 0xf100, OP_MOVE_QUICK, 0, 0, OMIT_B|OMIT_W, DEFAULT_L },
  { "muls", 0xc1c0, 0xf1c0, OP_DIV_MUL, MODE_AN, 0, OMIT_B, 0 },
  { "mulu", 0xc0c0, 0xf1c0, OP_DIV_MUL, MODE_AN, 0, OMIT_B, 0 },
  { "nbcd", 0x4800, 0xffc0, OP_SINGLE_EA_NO_SIZE, 0, MODE_AN|MODE_D16_PC|MODE_D8_PC_XN, OMIT_W|OMIT_L, DEFAULT_B },
  { "neg", 0x4400, 0xff00, OP_SINGLE_EA, 0, MODE_AN|MODE_D16_PC|MODE_D8_PC_XN, 0, 0 },
  { "negx", 0x4000, 0xff00, OP_SINGLE_EA, 0, MODE_AN|MODE_D16_PC|MODE_D8_PC_XN, 0, 0 },
  { "nop", 0x4e71, 0xffff, OP_NONE, 0, 0, NO_SIZE, 0 },
  { "not", 0x4600, 0xff00, OP_SINGLE_EA, 0, MODE_AN|MODE_D16_PC|MODE_D8_PC_XN, 0, 0 },
  { "or", 0x8000, 0xf000, OP_REG_AND_EA, MODE_AN, MODE_DN|MODE_AN|MODE_D16_PC|MODE_D8_PC_XN, 0, 0 },
  { "ori", 0x0000, 0xff00, OP_IMMEDIATE, 0, MODE_AN|MODE_D16_PC|MODE_D8_PC_XN, 0, 0 },
  { "ori", 0x003c, 0xffff, OP_LOGIC_CCR, 0, 0, OMIT_W|OMIT_L, DEFAULT_B },
  { "ori", 0x007c, 0xffff, OP_LOGIC_SR, 0, 0, OMIT_W|OMIT_L, DEFAULT_W },
  { "pea", 0x4840, 0xffc0, OP_SINGLE_EA_NO_SIZE, MODE_DN|MODE_AN|MODE_AN_P|MODE_AN_N, 0, NO_SIZE, 0 },
  { "reset", 0x4e70, 0xffff, OP_NONE, 0, 0, NO_SIZE, 0 },
  { "rol", 0xe7c0, 0xffc0, OP_SHIFT_EA, 0, MODE_DN|MODE_AN|MODE_D16_PC|MODE_D8_PC_XN, OMIT_B|OMIT_L, DEFAULT_W  },
  { "rol", 0xe118, 0xf118, OP_SHIFT, 0, 0, 0, 0 },
  { "ror", 0xe6c0, 0xffc0, OP_SHIFT_EA, 0, MODE_DN|MODE_AN|MODE_D16_PC|MODE_D8_PC_XN, OMIT_B|OMIT_L, DEFAULT_W },
  { "ror", 0xe018, 0xf118, OP_SHIFT, 0, 0, 0, 0 },
  { "roxl", 0xe5c0, 0xffc0, OP_SHIFT_EA, 0, MODE_DN|MODE_AN|MODE_D16_PC|MODE_D8_PC_XN, OMIT_B|OMIT_L, DEFAULT_W },
  { "roxl", 0xe110, 0xf118, OP_SHIFT, 0, 0, 0, 0 },
  { "roxr", 0xe4c0, 0xffc0, OP_SHIFT_EA, 0, MODE_DN|MODE_AN|MODE_D16_PC|MODE_D8_PC_XN, OMIT_B|OMIT_L, DEFAULT_W },
  { "roxr", 0xe010, 0xf118, OP_SHIFT, 0, 0, 0, 0 },
  { "rte", 0x4e73, 0xffff, OP_NONE, 0, 0, NO_SIZE, 0 },
  { "rtm", 0x06c0, 0xfff0, OP_REG, 0, 0, NO_SIZE, 0 },
  { "rtr", 0x4e77, 0xffff, OP_NONE, 0, 0, NO_SIZE, 0 },
  { "rts", 0x4e75, 0xffff, OP_NONE, 0, 0, NO_SIZE, 0 },
  { "sbcd", 0x8100, 0xf1f0, OP_BCD, 0, 0, OMIT_W|OMIT_L, DEFAULT_B },
  { "sub", 0x9000, 0xf000, OP_REG_AND_EA, 0, MODE_DN|MODE_AN, 0, 0 },
  { "suba", 0x9000, 0xf000, OP_EA_AREG, 0, 0, OMIT_B, 0 },
  { "subi", 0x0400, 0xff00, OP_IMMEDIATE, 0, MODE_AN|MODE_D16_PC|MODE_D8_PC_XN, 0, 0 },
  { "subq", 0x5100, 0xf100, OP_QUICK, 0, MODE_D16_PC|MODE_D8_PC_XN, 0, 0 },
  { "subx", 0x9100, 0xf130, OP_EXTENDED, 0, 0, 0, 0 },
  { "swap", 0x4840, 0xfff8, OP_REG, 0, 0, OMIT_B|OMIT_L, DEFAULT_W },
  { "tas", 0x4ac0, 0xffc0, OP_SINGLE_EA_NO_SIZE, 0, MODE_AN|MODE_D16_PC|MODE_D8_PC_XN, NO_SIZE, 0 },
  { "trap", 0x4e40, 0xfff0, OP_VECTOR, 0, 0, NO_SIZE, 0 },
  { "trapv", 0x4e76, 0xffff, OP_NONE, 0, 0, NO_SIZE, 0 },
  { "tst", 0x4a00, 0xff00, OP_SINGLE_EA, 0, 0, 0, 0 },
  { "unlk", 0x4e58, 0xfff8, OP_AREG, 0, 0, NO_SIZE, 0 },
  { NULL, 0x0000, 0x0000, OP_NONE },
};

char *table_68000_condition_codes[] =
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
STOP
UNPK
*/

uint16_t reverse_bits16(uint16_t bits)
{
  uint16_t num = 0;
  int n;

  uint8_t bit_table[] =
  {
    0, 8, 4, 12, 2, 10, 6, 14, 1, 9, 5, 13, 3, 11, 7, 15,
  };

  for (n = 0; n < 16; n += 4)
  {
    num = (num << 4) | bit_table[(bits >> n) & 0xf];
  }

  return num;
}



