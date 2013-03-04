/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2012 by Michael Kohn
 *
 */

#ifndef _TABLE_680x0_H
#define _TABLE_680x0_H

#include "assembler.h"

struct _table_680x0_no_operands
{
  char *instr;
  unsigned short int opcode;
};

enum
{
  OP_NONE,
  OP_EA,
  OP_LABEL,
  //OP_SRC,
  //OP_DEST,
  OP_IMM,
  OP_D,
  OP_A,
  OP_CCR,
  OP_SR,
  OP_DX,
  OP_DY,
  OP_AX,
  OP_AY,
  OP_IMM8,
  OP_AX_PLUS,
  OP_AY_PLUS,
  OP_DR_DQ,
  OP_DQ_DR_DQ,
  OP_SEA,
  OP_DEA,
  OP_REG_LIST,
  OP_D16_AY,
  OP_IMM256,
  OP_DL_DH_DL,
  OP_IMM16,
  OP_8BITDISP,
};

struct _680x0_operand
{
  unsigned char type;
  unsigned char pos;
  unsigned char size;
};

struct _table_680x0
{
  char *name;
  unsigned int opcode;
  unsigned int mask;
  char size;
  char size_pos;      // bit pos
  char cond_pos;      // str pos: Bcc would be 1, DBcc would be 2, etc
  //char mode_flag;
  struct _680x0_operand operands[3];
};

extern struct _table_680x0 table_680x0[];

#endif


