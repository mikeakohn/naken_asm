/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2019 by Michael Kohn
 *
 */

#ifndef NAKEN_ASM_TABLE_THUMB_H
#define NAKEN_ASM_TABLE_THUMB_H

#include <stdint.h>

#include "common/assembler.h"

enum
{
  OP_NONE,
  OP_SHIFT,
  OP_ADD_SUB,
  OP_REG_IMM,
  OP_ALU,
  OP_HI,
  OP_HI_BX,
  OP_PC_RELATIVE_LOAD,
  OP_LOAD_STORE,
  OP_LOAD_STORE_SIGN_EXT_HALF_WORD,
  OP_LOAD_STORE_IMM_OFFSET_WORD,
  OP_LOAD_STORE_IMM_OFFSET,
  OP_LOAD_STORE_IMM_OFFSET_HALF_WORD,
  OP_LOAD_STORE_SP_RELATIVE,
  OP_LOAD_ADDRESS,
  OP_ADD_OFFSET_TO_SP,
  OP_PUSH_POP_REGISTERS,
  OP_MULTIPLE_LOAD_STORE,
  OP_CONDITIONAL_BRANCH,
  OP_SOFTWARE_INTERRUPT,
  OP_UNCONDITIONAL_BRANCH,
  OP_LONG_BRANCH_WITH_LINK,
  OP_SP_SP_IMM,
  OP_REG_REG,
  OP_CPS,
  OP_UINT8,
  OP_REGISTER_ADDRESS,
  OP_MRS,
  OP_MSR,
  OP_REG_LOW,
};

enum
{
  VERSION_THUMB,
  VERSION_THUMB_2,
};

struct _table_thumb
{
  char *instr;
  uint16_t opcode;
  uint16_t mask;
  uint8_t type;
  uint8_t version;
  int8_t cycles;
};

struct _special_reg_thumb
{
  char *name;
  uint8_t value;
};

extern struct _table_thumb table_thumb[];
extern struct _special_reg_thumb special_reg_thumb[];

#endif

