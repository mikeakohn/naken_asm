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

#ifndef _TABLE_680x0_H
#define _TABLE_680x0_H

#include "assembler.h"

enum
{
  OP_NONE,
  OP_SINGLE_EA,
  OP_SINGLE_EA_NO_SIZE,
  OP_IMMEDIATE,
  OP_SHIFT_EA,
  OP_SHIFT,
  OP_REG_AND_EA,
};

struct _table_680x0_no_operands
{
  char *instr;
  unsigned short int opcode;
};

struct _table_680x0
{
  char *instr;
  unsigned short int opcode;
  unsigned short int mask;
  char type;
};

#if 0

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

#endif


