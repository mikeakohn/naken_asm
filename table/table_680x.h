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

#ifndef _TABLE_680X
#define _TABLE_680X

enum
{
  M6800_OP_UNDEF,
  M6800_OP_NONE,
  M6800_OP_IMM8,
  M6800_OP_IMM16,
  M6800_OP_DIR_PAGE_8,
  M6800_OP_ABSOLUTE_16,
  M6800_OP_NN_X,
  M6800_OP_REL_OFFSET,
};

struct _m680x_table
{
  const char *instr;
  char operand_type;
};

#endif

