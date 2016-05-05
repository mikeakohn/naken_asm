/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2016 by Michael Kohn
 *
 */

#ifndef _TABLE_MIPS32_H
#define _TABLE_MIPS32_H

#include <stdint.h>

#define FORMAT_SPECIAL0 0x00
#define FORMAT_SPECIAL2 0x1c
#define FORMAT_SPECIAL3 0x1f

#define MIPS_II 1
#define MIPS_III 2
#define MIPS_IV 4

enum
{
  SPECIAL_TYPE_REGS,
  SPECIAL_TYPE_SA,
  SPECIAL_TYPE_BITS,
  SPECIAL_TYPE_BITS2,
};

enum
{
  MIPS_OP_NONE,
  MIPS_OP_RD,
  MIPS_OP_RS,
  MIPS_OP_RT,
  MIPS_OP_SA,
  MIPS_OP_LABEL,
  MIPS_OP_IMMEDIATE,
  MIPS_OP_IMMEDIATE_SIGNED,
  MIPS_OP_IMMEDIATE_RS,
  //MIPS_OP_RT_IS_0,
  //MIPS_OP_RT_IS_1,
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
  uint8_t function;
  int8_t operand_count;
};

struct _mips_branch
{
  const char *instr;
  uint8_t opcode;
  int8_t op_rt;
  uint16_t version;
};

struct _mips_cop_instr
{
  const char *instr;
  char operand[3];
  uint8_t function;
  uint8_t format;
  int8_t operand_count;
};

struct _mips_special_instr
{
  const char *instr;
  char operand[4];
  int8_t operand_count;
  uint8_t format;
  uint8_t operation;
  uint8_t function;
  uint8_t type;
};

extern struct _mips_instr mips_r_table[];
extern struct _mips_instr mips_i_table[];
extern struct _mips_branch mips_branch_table[];
extern struct _mips_special_instr mips_special_table[];
extern struct _mips_cop_instr mips_cop_table[];

#endif

