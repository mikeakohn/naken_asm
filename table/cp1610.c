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

#include <stdlib.h>

#include "table/cp1610.h"

struct _table_cp1610 table_cp1610[] =
{
  { "hlt",  0x0000, 0xffff, CP1610_OP_NONE, 1, 1 },
  { "sdbd", 0x0001, 0xffff, CP1610_OP_NONE, 4, 4 },
  { "eis",  0x0002, 0xffff, CP1610_OP_NONE, 4, 4 },
  { "dis",  0x0003, 0xffff, CP1610_OP_NONE, 4, 4 },
  { "j",    0x0004, 0xffff, CP1610_OP_JUMP, 12, 12 },
  { "je",   0x0004, 0xffff, CP1610_OP_JUMP, 12, 12 },
  { "jd",   0x0004, 0xffff, CP1610_OP_JUMP, 12, 12 },
  { "jsr",  0x0004, 0xffff, CP1610_OP_JSR, 12, 12 },
  { "jsre", 0x0004, 0xffff, CP1610_OP_JSR, 12, 12 },
  { "jsrd", 0x0004, 0xffff, CP1610_OP_JSR, 12, 12 },
  { "tci",  0x0005, 0xffff, CP1610_OP_NONE, 4, 4 },
  { "clrc", 0x0006, 0xffff, CP1610_OP_NONE, 4, 4 },
  { "setc", 0x0007, 0xffff, CP1610_OP_NONE, 4, 4 },
  { "incr", 0x0008, 0xfff8, CP1610_OP_REG, 6, 6 },
  { "decr", 0x0010, 0xfff8, CP1610_OP_REG, 6, 6 },
  { "comr", 0x0018, 0xfff8, CP1610_OP_REG, 6, 6 },
  { "negr", 0x0020, 0xfff8, CP1610_OP_REG, 6, 6 },
  { "adcr", 0x0028, 0xfff8, CP1610_OP_REG, 6, 6 },
  { "gswd", 0x0030, 0xfffc, CP1610_OP_REG, 6, 6 },
  { "nop" , 0x0034, 0xffff, CP1610_OP_NONE, 6, 6 },
  { "nop" , 0x0035, 0xffff, CP1610_OP_NONE, 6, 6 },
  { "sin" , 0x0036, 0xffff, CP1610_OP_NONE, 6, 6 },
  { "sin" , 0x0037, 0xffff, CP1610_OP_NONE, 6, 6 },
  { "swap", 0x0040, 0xffc0, CP1610_OP_1OP, 6, 8 },
  { "sll",  0x0048, 0xffc0, CP1610_OP_1OP, 6, 8 },
  { "rlc",  0x0050, 0xffc0, CP1610_OP_1OP, 6, 8 },
  { "sllc", 0x0058, 0xffc0, CP1610_OP_1OP, 6, 8 },
  { "slr",  0x0060, 0xffc0, CP1610_OP_1OP, 6, 8 },
  { "sar",  0x0068, 0xffc0, CP1610_OP_1OP, 6, 8 },
  { "rrc",  0x0070, 0xffc0, CP1610_OP_1OP, 6, 8 },
  { "sarc", 0x0078, 0xffc0, CP1610_OP_1OP, 6, 8 },
  { "jr",   0x0087, 0xffc7, CP1610_OP_JR, 7, 7 },
  { "movr", 0x0080, 0xffc0, CP1610_OP_SREG_DREG, 6, 7 },
  { "addr", 0x00c0, 0xffc0, CP1610_OP_SREG_DREG, 6, 6 },
  { "subr", 0x0100, 0xffc0, CP1610_OP_SREG_DREG, 6, 6 },
  { "cmpr", 0x0140, 0xffc0, CP1610_OP_SREG_DREG, 6, 6 },
  { "andr", 0x0180, 0xffc0, CP1610_OP_SREG_DREG, 6, 6 },
  { "xorr", 0x01c0, 0xffc0, CP1610_OP_SREG_DREG, 6, 6 },
  { "b",    0x0200, 0xffdf, CP1610_OP_BRANCH, 6, 6 },
  { "bc",   0x0201, 0xffdf, CP1610_OP_BRANCH, 6, 6 },
  { "bov",  0x0202, 0xffdf, CP1610_OP_BRANCH, 6, 6 },
  { "bpl",  0x0203, 0xffdf, CP1610_OP_BRANCH, 6, 6 },
  { "beq",  0x0204, 0xffdf, CP1610_OP_BRANCH, 6, 6 },
  { "bze",  0x0204, 0xffdf, CP1610_OP_BRANCH, 6, 6 },
  { "blt",  0x0205, 0xffdf, CP1610_OP_BRANCH, 6, 6 },
  { "bnge", 0x0205, 0xffdf, CP1610_OP_BRANCH, 6, 6 },
  { "ble",  0x0206, 0xffdf, CP1610_OP_BRANCH, 6, 6 },
  { "bngt", 0x0206, 0xffdf, CP1610_OP_BRANCH, 6, 6 },
  { "busc", 0x0207, 0xffdf, CP1610_OP_BRANCH, 6, 6 },
  { "nopp", 0x0208, 0xffdf, CP1610_OP_BRANCH, 6, 6 },
  { "bnc",  0x0209, 0xffdf, CP1610_OP_BRANCH, 6, 6 },
  { "bnov", 0x020a, 0xffdf, CP1610_OP_BRANCH, 6, 6 },
  { "bmi",  0x020b, 0xffdf, CP1610_OP_BRANCH, 6, 6 },
  { "bneq", 0x020c, 0xffdf, CP1610_OP_BRANCH, 6, 6 },
  { "bnze", 0x020c, 0xffdf, CP1610_OP_BRANCH, 6, 6 },
  { "bge",  0x020d, 0xffdf, CP1610_OP_BRANCH, 6, 6 },
  { "bnlt", 0x020d, 0xffdf, CP1610_OP_BRANCH, 6, 6 },
  { "bgt",  0x020e, 0xffdf, CP1610_OP_BRANCH, 6, 6 },
  { "bnle", 0x020e, 0xffdf, CP1610_OP_BRANCH, 6, 6 },
  { "besc", 0x020f, 0xffdf, CP1610_OP_BRANCH, 6, 6 },
  { "mvo",  0x0240, 0xfff8, CP1610_OP_REG_ADDRESS, 11, 11 },
  { "mvoi", 0x0278, 0xfff8, CP1610_OP_REG_IMMEDIATE, 9, 9 },
  { "mvo@", 0x0240, 0xffc0, CP1610_OP_AREG_SREG, 9, 9 },
  { "mvi",  0x0280, 0xfff8, CP1610_OP_ADDRESS_REG, 10, 10 },
  { "mvii", 0x02b8, 0xfff8, CP1610_OP_IMMEDIATE_REG, 8, 10 },
  { "mvi@", 0x0280, 0xffc0, CP1610_OP_SREG_DREG, 8, 11 },
  { "pshr", 0x0270, 0xfff8, CP1610_OP_REG, 6, 6 },
  { "pulr", 0x02b0, 0xfff8, CP1610_OP_REG, 6, 6 },
  { "add",  0x02c0, 0xfff8, CP1610_OP_ADDRESS_REG, 10, 10 },
  { "addi", 0x02f8, 0xfff8, CP1610_OP_IMMEDIATE_REG, 8, 10 },
  { "add@", 0x02c0, 0xffc0, CP1610_OP_SREG_DREG, 8, 11 },
  { "sub",  0x0300, 0xfff8, CP1610_OP_ADDRESS_REG, 10, 10 },
  { "subi", 0x0338, 0xfff8, CP1610_OP_IMMEDIATE_REG, 8, 10 },
  { "sub@", 0x0300, 0xffc0, CP1610_OP_SREG_DREG, 8, 11 },
  { "cmp",  0x0340, 0xfff8, CP1610_OP_ADDRESS_REG, 10, 10 },
  { "cmpi", 0x0378, 0xfff8, CP1610_OP_IMMEDIATE_REG, 8, 10 },
  { "cmp@", 0x0340, 0xffc0, CP1610_OP_SREG_DREG, 8, 11 },
  { "and",  0x0380, 0xfff8, CP1610_OP_ADDRESS_REG, 10, 10 },
  { "andi", 0x03b8, 0xfff8, CP1610_OP_IMMEDIATE_REG, 8, 10 },
  { "and@", 0x0380, 0xffc0, CP1610_OP_SREG_DREG, 8, 11 },
  { "xor",  0x03c0, 0xfff8, CP1610_OP_ADDRESS_REG, 10, 10 },
  { "xori", 0x03f8, 0xfff8, CP1610_OP_IMMEDIATE_REG, 8, 10 },
  { "xor@", 0x03c0, 0xffc0, CP1610_OP_SREG_DREG, 8, 11 },
  { "clrr", 0x01c0, 0xffc0, CP1610_OP_REG_X2, 6, 6 },
  { NULL,   0x0000, 0x00ff, CP1610_OP_ILLEGAL, 0, 0 },
};

struct _table_cp1610_jump table_cp1610_jump[] =
{
  { "j",    3, 0, 0 },
  { "je",   3, 1, 0 },
  { "jd",   3, 2, 0 },
  { "jsr",  0, 0, 1 },
  { "jsre", 0, 1, 1 },
  { "jsrd", 0, 2, 1 },
};

