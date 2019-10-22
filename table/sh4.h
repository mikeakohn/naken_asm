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

#ifndef NAKEN_ASM_TABLE_SH4_H
#define NAKEN_ASM_TABLE_SH4_H

#include "common/assembler.h"

enum
{
  OP_NONE,
  OP_REG,
  OP_FREG,
  OP_DREG,
  OP_REG_REG,
  OP_FREG_FREG,
  OP_DREG_DREG,
  OP_DREG_XDREG,
  OP_XDREG_DREG,
  OP_XDREG_XDREG,
  OP_FVREG_FVREG,
  OP_IMM_REG,
  OP_IMM_R0,
  OP_IMM_AT_R0_GBR,
  OP_BRANCH_S9,
  OP_BRANCH_S13,
  OP_FREG_FPUL,
  OP_DREG_FPUL,
  OP_FPUL_FREG,
  OP_FPUL_DREG,
  OP_FR0_FREG_FREG,
  OP_FREG_AT_REG,
  OP_DREG_AT_REG,
  OP_FREG_AT_MINUS_REG,
  OP_DREG_AT_MINUS_REG,
  OP_FREG_AT_R0_REG,
  OP_DREG_AT_R0_REG,
};

// This needs to be in sync with the table/sh4.h SH4_ enums.
enum
{
  SH4_NA,
  SH4_REG,
  SH4_FREG,
  SH4_DREG,
  SH4_XDREG,
  SH4_FVREG,
  SH4_AT_REG,
  SH4_AT_MINUS_REG,
  SH4_AT_REG_PLUS,
};

struct _table_sh4
{
  char *instr;
  uint16_t opcode;
  uint16_t mask;
  uint8_t type;
};

struct _operand_type_sh4
{
  uint8_t count;
  uint8_t type_0;
  uint8_t shift_0;
  uint8_t type_1;
  uint8_t shift_1;
};

extern struct _table_sh4 table_sh4[];
extern struct _operand_type_sh4 operand_type_sh4[];

#endif

