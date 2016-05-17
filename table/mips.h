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

#ifndef _TABLE_MIPS_H
#define _TABLE_MIPS_H

#include <stdint.h>

#define FORMAT_SPECIAL0 0x00
#define FORMAT_SPECIAL2 0x1c
#define FORMAT_SPECIAL3 0x1f

#define MIPS_I 0
#define MIPS_II 1
#define MIPS_III 2
#define MIPS_IV 4
#define MIPS_32 8
#define MIPS_FPU 16 
#define MIPS_EE_CORE 32

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
  MIPS_OP_PREG,
  MIPS_OP_HINT,
  MIPS_OP_CACHE,
  MIPS_OP_FD,
  MIPS_OP_FS,
  MIPS_OP_FT,
};

#if 0
enum
{
  MIPS_COP_NONE,
  MIPS_COP_FD,
  MIPS_COP_FS,
  MIPS_COP_FT,
};
#endif

struct _mips_instr
{
  const char *instr;
  char operand[3];
  uint8_t function;
  int8_t operand_count;
  uint16_t version;
};

struct _mips_branch
{
  const char *instr;
  uint8_t opcode;
  int8_t op_rt;
  uint16_t version;
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
  uint16_t version;
};

struct _mips_no_operands
{
  const char *instr;
  uint32_t opcode;
  uint16_t version;
};

struct _mips_other
{
  const char *instr;
  char operand[3];
  int8_t operand_count;
  uint32_t opcode;
  uint32_t mask;
  uint16_t version;
};

struct _mips_cache
{
  const char *name;
  uint8_t op;
};

extern struct _mips_instr mips_r_table[];
extern struct _mips_instr mips_i_table[];
extern struct _mips_branch mips_branch_table[];
extern struct _mips_special_instr mips_special_table[];
extern struct _mips_no_operands mips_no_operands[];
extern struct _mips_other mips_other[];
extern struct _mips_cache mips_cache[];

#endif

