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
  OP_XDREG_AT_REG,
  OP_XDREG_AT_MINUS_REG,
  OP_XDREG_AT_R0_REG,
  OP_AT_REG_DREG,
  OP_AT_REG_PLUS_DREG,
  OP_AT_R0_REG_DREG,
  OP_AT_REG_FREG,
  OP_AT_REG_PLUS_FREG,
  OP_AT_R0_REG_FREG,
  OP_AT_REG_XDREG,
  OP_AT_REG_PLUS_XDREG,
  OP_AT_R0_REG_XDREG,
  OP_XMTRX_FVREG,
  OP_AT_REG,
  OP_REG_SPECIAL,
  OP_REG_REG_BANK,
  OP_AT_REG_PLUS_SPECIAL,
  OP_AT_REG_PLUS_REG_BANK,
  OP_AT_REG_PLUS_AT_REG_PLUS,
  OP_REG_AT_REG,
  OP_REG_AT_MINUS_REG,
  OP_REG_AT_R0_REG,
  OP_R0_AT_DISP_GBR,
  OP_R0_AT_DISP_REG,
  OP_AT_REG_REG,
  OP_AT_REG_PLUS_REG,
  OP_AT_R0_REG_REG,
  OP_AT_DISP_GBR_R0,
  OP_AT_DISP_REG_R0,
  OP_AT_DISP_PC_REG,
  OP_AT_DISP_REG_REG,
  OP_AT_DISP_PC_R0,
  OP_R0_AT_REG,
  OP_SPECIAL_REG,
  OP_REG_BANK_REG,
  OP_SPECIAL_AT_MINUS_REG,
  OP_REG_BANK_AT_MINUS_REG,
  OP_IMM,
  OP_REG_AT_DISP_REG,
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
  SH4_AT_R0_REG,
  SH4_SPECIAL,
  SH4_REG_BANK,
};

struct _table_sh4
{
  char *instr;
  uint16_t opcode;
  uint16_t mask;
  uint8_t type;
  uint8_t special;
};

enum
{
  SPECIAL_REG_NONE,
  SPECIAL_REG_FPUL,
  SPECIAL_REG_GBR,
  SPECIAL_REG_SR,
  SPECIAL_REG_VBR,
  SPECIAL_REG_SSR,
  SPECIAL_REG_SPC,
  SPECIAL_REG_DBR,
  SPECIAL_REG_FPSCR,
  SPECIAL_REG_MACL,
  SPECIAL_REG_MACH,
  SPECIAL_REG_PR,
  SPECIAL_REG_SGR,
  SPECIAL_REG_XMTRX,
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
extern char *sh4_specials[];

#endif

