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

#ifndef NAKEN_ASM_TABLE_68000_H
#define NAKEN_ASM_TABLE_68000_H

#include <stdint.h>

#include "common/assembler.h"

enum
{
  OP_NONE,
  OP_SINGLE_EA,
  OP_SINGLE_EA_NO_SIZE,
  //OP_SINGLE_EA_TO_ADDR,
  OP_IMMEDIATE,
  OP_SHIFT_EA,
  OP_SHIFT,
  OP_REG_AND_EA,
  OP_VECTOR,
  OP_VECTOR3,
  OP_AREG,
  OP_REG,
  OP_EA_AREG,
  OP_EA_DREG,
  OP_LOAD_EA,
  OP_QUICK,
  OP_MOVE_QUICK,
  OP_MOVE_FROM_CCR,
  OP_MOVE_TO_CCR,
  OP_MOVE_FROM_SR,
  OP_MOVE_TO_SR,
  OP_MOVEA,
  OP_CMPM,
  OP_BCD,
  OP_EXTENDED,
  OP_ROX_MEM,
  OP_ROX,
  OP_EXCHANGE,
  OP_BIT_REG_EA,
  OP_BIT_IMM_EA,
  OP_EA_DREG_WL,
  OP_LOGIC_CCR,
  OP_LOGIC_SR,
  OP_BRANCH,
  OP_EXT,
  OP_LINK_W,
  OP_LINK_L,
  OP_DIV_MUL,
  OP_MOVEP,
  OP_MOVEM,
  OP_MOVE,
  OP_JUMP,
  OP_DREG_EA,
};

#define MODE_DN 1
#define MODE_AN 2
#define MODE_AN_P 4
#define MODE_AN_N 8
#define MODE_IMM 16
#define MODE_D16_PC 32
#define MODE_D8_PC_XN 64
#define OMIT_B 1
#define OMIT_W 2
#define OMIT_L 4
#define NO_SIZE 128
#define DEFAULT_B 1
#define DEFAULT_W 2
#define DEFAULT_L 3

struct _table_68000
{
  char *instr;
  uint16_t opcode;
  uint16_t mask;
  uint8_t type;
  uint8_t omit_src;
  uint8_t omit_dst;
  uint8_t omit_size;
  uint8_t default_size;
};

uint16_t reverse_bits16(uint16_t bits);

extern struct _table_68000 table_68000[];
extern char *table_68000_condition_codes[];

#endif

