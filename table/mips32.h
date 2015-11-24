/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2015 by Michael Kohn
 *
 */

#ifndef _TABLE_MIPS
#define _TABLE_MIPS

enum
{
  MIPS_OP_NONE,
  MIPS_OP_RD,
  MIPS_OP_RS,
  MIPS_OP_RT,
  MIPS_OP_SA,
  MIPS_OP_LABEL,
  MIPS_OP_IMMEDIATE,
  MIPS_OP_IMMEDIATE_RS,
  MIPS_OP_RT_IS_0,
  MIPS_OP_RT_IS_1,
};

enum
{
  MIPS_COP_NONE,
  MIPS_COP_FD,
  MIPS_COP_FS,
  MIPS_COP_FT,
};

struct _mips32_instr
{
  const char *instr;
  char operand[3];
  unsigned char function;
  char operand_count;
};

struct _mips32_cop_instr
{
  const char *instr;
  char operand[3];
  unsigned char function;
  unsigned char format;
  char operand_count;
};

#endif

