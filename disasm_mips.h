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

#ifndef _DISASM_MIPS_H
#define _DISASM_MIPS_H

#include "assembler.h"

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

struct _mips_instr
{
  const char *instr;
  char operand[3];
  unsigned char function;
  char operand_count;
};

struct _mips_cop_instr
{
  const char *instr;
  char operand[3];
  unsigned char function;
  unsigned char format;
  char operand_count;
};

extern struct _mips_instr mips_r_table[];
extern struct _mips_instr mips_i_table[];
extern struct _mips_cop_instr mips_cop_table[];

int get_cycle_count_mips(unsigned short int opcode);
int disasm_mips(struct _memory *memory, int address, char *instruction, int *cycles_min, int *cycles_max);
void list_output_mips(struct _asm_context *asm_context, int address);
void disasm_range_mips(struct _memory *memory, int start, int end);

#endif

