/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2013 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "table_680x0.h"

struct _table_680x0 table_680x0[] =
{
  { "abcd", 0xc100, 0xf1f0, OP_BCD, 0, 0, OMIT_W|OMIT_L },
  { "add", 0xd000, 0xf000, OP_REG_AND_EA, 0, MODE_DN|MODE_AN, 0 },
  { "adda", 0xd000, 0xf000, OP_EA_AREG, 0, 0, OMIT_B },
  { "addi", 0x0600, 0xff00, OP_IMMEDIATE, 0, MODE_AN, 0 },
  { "addq", 0x5000, 0xf100, OP_QUICK, 0, 0, 0 },
  { "addx", 0xd100, 0xf130, OP_EXTENDED, 0, 0, 0 },
  { "and", 0xc000, 0xf000, OP_REG_AND_EA, MODE_AN, MODE_DN|MODE_AN, 0 },
  { "andi", 0x0200, 0xff00, OP_IMMEDIATE, 0, MODE_AN, 0 },
  { "andi", 0x023c, 0xffff, OP_LOGIC_CCR, 0, 0, OMIT_W|OMIT_L },
  { "asl", 0xe1c0, 0xffc0, OP_SHIFT_EA, 0, MODE_DN|MODE_AN, 0 },
  { "asl", 0xe100, 0xf118, OP_SHIFT, 0, 0, 0 },
  { "asr", 0xe0c0, 0xffc0, OP_SHIFT_EA, 0, MODE_DN|MODE_AN, 0 },
  { "asr", 0xe000, 0xf118, OP_SHIFT, 0, 0, 0 },
  { "bchg", 0x0140, 0xf1c0, OP_REG_EA_NO_SIZE, 0, MODE_AN, OMIT_W },
  { "bchg", 0x0840, 0xffc0, OP_EXTRA_IMM_EA, 0, MODE_AN, OMIT_W },
  { "bclr", 0x0180, 0xf1c0, OP_REG_EA_NO_SIZE, 0, MODE_AN, OMIT_W },
  { "bclr", 0x0880, 0xffc0, OP_EXTRA_IMM_EA, 0, MODE_AN, OMIT_W },
  { "bkpt", 0x4848, 0xfff8, OP_VECTOR3, 0, 0, NO_SIZE },
  { "bsr", 0x6100, 0xff00, OP_BRANCH, 0, 0, 0 },
  { "bra", 0x6000, 0xff00, OP_BRANCH, 0, 0, 0 },
  { "bset", 0x01c0, 0xf1c0, OP_REG_EA_NO_SIZE, 0, MODE_AN, OMIT_W },
  { "bset", 0x08c0, 0xffc0, OP_EXTRA_IMM_EA, 0, MODE_AN, OMIT_W },
  { "btst", 0x0100, 0xf1c0, OP_REG_EA_NO_SIZE, 0, MODE_AN, OMIT_W },
  { "btst", 0x0800, 0xffc0, OP_EXTRA_IMM_EA, 0, MODE_AN, OMIT_W },
  { "chk", 0x4000, 0xf040, OP_EA_DREG_WL, MODE_AN, 0, OMIT_B },
  { "clr", 0x4200, 0xff00, OP_SINGLE_EA, 0, MODE_AN|MODE_IMM, 0 },
  { "cmp", 0xb000, 0xf000, OP_EA_DREG, 0, 0, 0 },
  { "cmpa", 0xb000, 0xf000, OP_EA_AREG, 0, 0, OMIT_B },
  { "cmpi", 0x0c00, 0xff00, OP_IMMEDIATE, 0, MODE_AN, 0 },
  { "cmpm", 0xb108, 0xf138, OP_CMPM, 0, 0, 0 },
  { "divs", 0x81c0, 0xf1c0, OP_DIV_MUL, MODE_AN, 0, OMIT_B },
  { "divu", 0x80c0, 0xf1c0, OP_DIV_MUL, MODE_AN, 0, OMIT_B },
  { "eor", 0xb000, 0xf000, OP_REG_AND_EA, 0, MODE_AN, 0 },
  { "eori", 0x0900, 0xff00, OP_IMMEDIATE, 0, MODE_AN, 0 },
  { "eori", 0x0a3c, 0xffff, OP_LOGIC_CCR, 0, 0, OMIT_W|OMIT_L },
  { "exg", 0xc100, 0xf100, OP_EXCHANGE, 0, 0, OMIT_B|OMIT_W },
  { "ext", 0x4800, 0xfe38, OP_EXT, 0, 0, OMIT_B },
  { "illegal", 0x4afc, 0xffff, OP_NONE, 0, 0, NO_SIZE },
  { "jmp", 0x4ec0, 0xffc0, OP_JUMP, 0, MODE_DN|MODE_AN|MODE_AN_P|MODE_AN_N, NO_SIZE },
  { "jsr", 0x4e80, 0xffc0, OP_JUMP,  MODE_DN|MODE_AN|MODE_AN_P|MODE_AN_N, NO_SIZE },
  { "lea", 0x41c0, 0xf1c0, OP_LOAD_EA, MODE_DN|MODE_AN|MODE_AN_P|MODE_AN_N|MODE_IMM, 0, OMIT_B|OMIT_W },
  { "link", 0x4e50, 0xfff8, OP_LINK, 0, 0, OMIT_B },
  { "lsl", 0xe3c0, 0xffc0, OP_SHIFT_EA, 0, MODE_DN|MODE_AN, 0 },
  { "lsl", 0xe108, 0xf118, OP_SHIFT, 0, 0, 0 },
  { "lsr", 0xe2c0, 0xffc0, OP_SHIFT_EA, 0, MODE_DN|MODE_AN, 0 },
  { "lsr", 0xe008, 0xf118, OP_SHIFT, 0, 0, 0 },
  { "move", 0x0000, 0xc000, OP_MOVE, 0, MODE_AN, 0 },
  { "move", 0x42c0, 0xffc0, OP_MOVE_FROM_CCR, 0, MODE_AN, OMIT_B|OMIT_L },
  { "move", 0x44c0, 0xffc0, OP_MOVE_TO_CCR, MODE_AN, 0, OMIT_B|OMIT_L },
  { "move", 0x40c0, 0xffc0, OP_MOVE_FROM_SR, 0, MODE_AN, OMIT_B|OMIT_L },
  { "movea", 0x0040, 0xc1c0, OP_MOVEA, 0, 0, OMIT_B },
  { "movem", 0x4880, 0xfb80, OP_MOVEM, MODE_DN|MODE_AN|MODE_AN_P|MODE_IMM, MODE_DN|MODE_AN|MODE_AN_N|MODE_IMM, OMIT_B },
  { "movep", 0x0008, 0xf038, OP_MOVEP, 0, 0, OMIT_B },
  { "moveq", 0x7000, 0xf100, OP_MOVE_QUICK, 0, 0, OMIT_B|OMIT_W },
  { "muls", 0xc1c0, 0xf1c0, OP_DIV_MUL, MODE_AN, 0, OMIT_B },
  { "mulu", 0xc1c0, 0xf1c0, OP_DIV_MUL, MODE_AN, 0, OMIT_B },
  { "nbcd", 0x4800, 0xffc0, OP_SINGLE_EA_NO_SIZE, 0, MODE_AN, OMIT_W|OMIT_L },
  { "neg", 0x4400, 0xff00, OP_SINGLE_EA, 0, MODE_AN, 0 },
  { "negx", 0x4000, 0xff00, OP_SINGLE_EA, 0, MODE_AN, 0 },
  { "nop", 0x4e71, 0xffff, OP_NONE, 0, 0, NO_SIZE },
  { "not", 0x4600, 0xff00, OP_SINGLE_EA, 0, MODE_AN, 0 },
  { "or", 0x8000, 0xf000, OP_REG_AND_EA, MODE_AN, MODE_DN|MODE_AN, 0 },
  { "ori", 0x0000, 0xff00, OP_IMMEDIATE, 0, MODE_AN, 0 },
  { "ori", 0x003c, 0xffff, OP_LOGIC_CCR, 0, 0, OMIT_W|OMIT_L },
  { "pea", 0x4840, 0xffc0, OP_SINGLE_EA_NO_SIZE, 0, MODE_DN|MODE_AN|MODE_AN_P|MODE_AN_N, NO_SIZE },
  { "reset", 0x4e70, 0xffff, OP_NONE, 0, 0, NO_SIZE },
  { "rol", 0xe7c0, 0xffc0, OP_SHIFT_EA, 0, MODE_DN|MODE_AN, 0  },
  { "rol", 0xe118, 0xf118, OP_SHIFT, 0, 0, 0 },
  { "ror", 0xe6c0, 0xffc0, OP_SHIFT_EA, 0, MODE_DN|MODE_AN, 0 },
  { "ror", 0xe018, 0xf118, OP_SHIFT, 0, 0, 0 },
  { "roxl", 0xe5c0, 0xffc0, OP_ROX_MEM, 0, MODE_DN|MODE_AN, 0 },
  { "roxl", 0xe130, 0xf138, OP_ROX, 0, 0, 0 },
  { "roxr", 0xe4c0, 0xffc0, OP_ROX_MEM, 0, MODE_DN|MODE_AN, 0 },
  { "roxr", 0xe110, 0xf138, OP_ROX, 0, 0, 0 },
  { "rte", 0x4e73, 0xffff, OP_NONE, 0, 0, NO_SIZE },
  { "rtm", 0x06c0, 0xfff0, OP_REG, 0, 0, NO_SIZE },
  { "rtr", 0x4e77, 0xffff, OP_NONE, 0, 0, NO_SIZE },
  { "rts", 0x4e75, 0xffff, OP_NONE, 0, 0, NO_SIZE },
  { "sbcd", 0x8100, 0xf1f0, OP_BCD, 0, 0, OMIT_W|OMIT_L },
  { "sub", 0x9000, 0xf000, OP_REG_AND_EA, 0, MODE_DN|MODE_AN, 0 },
  { "suba", 0x9000, 0xf000, OP_EA_AREG, 0, 0, OMIT_B },
  { "subi", 0x0400, 0xff00, OP_IMMEDIATE, 0, MODE_AN, 0 },
  { "subq", 0x5100, 0xf100, OP_QUICK, 0, 0, 0 },
  { "subx", 0x9100, 0xf130, OP_EXTENDED, 0, 0, 0 },
  { "swap", 0x4840, 0xfff8, OP_AREG, 0, 0, OMIT_B|OMIT_L },
  { "tas", 0x4ac0, 0xffc0, OP_SINGLE_EA_NO_SIZE, 0, MODE_AN, NO_SIZE },
  { "trap", 0x4e40, 0xfff0, OP_VECTOR, 0, 0, NO_SIZE },
  { "trapv", 0x4e76, 0xffff, OP_NONE, 0, 0, NO_SIZE },
  { "tst", 0x4a00, 0xff00, OP_SINGLE_EA, 0, 0, 0 },
  { "unlk", 0x4e58, 0xfff8, OP_AREG, 0, 0, NO_SIZE },
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



