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
};

struct _680x0_operand
{
  unsigned char op;
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
  char mode_flag;
  struct _680x0_operand operands[3];
};

extern struct _table_680x0 table_680x0[];

#endif


