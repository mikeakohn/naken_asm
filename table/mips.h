/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2017 by Michael Kohn
 *
 */

#ifndef _TABLE_MIPS_H
#define _TABLE_MIPS_H

#include <stdint.h>

#define FORMAT_SPECIAL0 0x00
#define FORMAT_SPECIAL2 0x1c
#define FORMAT_SPECIAL3 0x1f

#define MIPS_I 1
#define MIPS_II 2
#define MIPS_III 4
#define MIPS_IV 8
#define MIPS_32 16 
#define MIPS_FPU 32 
#define MIPS_EE_CORE 64
#define MIPS_EE_VU 128
#define MIPS_MSA 256

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
  MIPS_OP_WD,
  MIPS_OP_WS,
  MIPS_OP_WT,
  MIPS_OP_VFD,
  MIPS_OP_VFS,
  MIPS_OP_VFT,
  MIPS_OP_VID,
  MIPS_OP_VIS,
  MIPS_OP_VIT,
  MIPS_OP_I,
  MIPS_OP_Q,
  MIPS_OP_P,
  MIPS_OP_R,
  MIPS_OP_ACC,
  MIPS_OP_VI01,
  MIPS_OP_VI27,
  MIPS_OP_VBASE,
  MIPS_OP_VBASE_DEC,
  MIPS_OP_VBASE_INC,
  MIPS_OP_OFFSET_VBASE,
  MIPS_OP_IMMEDIATE15_2,
  MIPS_OP_IMMEDIATE5,
  MIPS_OP_OPTIONAL,
};

#define FLAG_NONE 0
#define FLAG_DEST 1
#define FLAG_BC 2
#define FLAG_XYZ 8
#define FLAG_TE 16
#define FLAG_SE 32

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
  int8_t operand[4];
  int8_t operand_count;
  uint8_t format;
  uint8_t operation;
  uint8_t function;
  uint8_t type;
  uint16_t version;
};

struct _mips_other
{
  const char *instr;
  int8_t operand[3];
  int8_t operand_count;
  uint32_t opcode;
  uint32_t mask;
  uint16_t version;
};

struct _mips_ee_vector
{
  const char *instr;
  int8_t operand[3];
  int8_t operand_count;
  uint32_t opcode;
  uint32_t mask;
  uint8_t flags;
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
extern struct _mips_other mips_other[];
extern struct _mips_cache mips_cache[];
extern struct _mips_other mips_msa[];
extern struct _mips_ee_vector mips_ee_vector[];

#endif

