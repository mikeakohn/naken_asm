/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2020 by Michael Kohn
 *
 */

#ifndef NAKEN_ASM_TABLE_6800_H
#define NAKEN_ASM_TABLE_6800_H

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

struct _table_6800
{
  const char *instr;
  char operand_type;
};

extern struct _table_6800 table_6800[];

#endif

