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

#ifndef NAKEN_ASM_TABLE_MSP430_H
#define NAKEN_ASM_TABLE_MSP430_H

#include <stdint.h>

#include "common/assembler.h"

enum
{
  OP_NONE,
  OP_ONE_OPERAND,
  OP_ONE_OPERAND_W,
  OP_ONE_OPERAND_X,
  OP_JUMP,
  OP_TWO_OPERAND,
  OP_MOVA_AT_REG_REG,
  OP_MOVA_AT_REG_PLUS_REG,
  OP_MOVA_ABS20_REG,
  OP_MOVA_INDEXED_REG,
  OP_SHIFT20,
  OP_MOVA_REG_ABS,
  OP_MOVA_REG_INDEXED,
  OP_IMMEDIATE_REG,
  OP_REG_REG,
  OP_CALLA_SOURCE,
  OP_CALLA_ABS20,
  OP_CALLA_INDIRECT_PC,
  OP_CALLA_IMMEDIATE,
  OP_PUSH,
  OP_POP,
  OP_X_ONE_OPERAND,
  OP_X_ONE_OPERAND_W,
  OP_X_TWO_OPERAND,
};

enum
{
  VERSION_MSP430,
  VERSION_MSP430X,
  VERSION_MSP430X_EXT,
};

struct _table_msp430
{
  char *instr;
  uint16_t opcode;
  uint16_t mask;
  uint8_t type;
  uint8_t version;
};

extern struct _table_msp430 table_msp430[];

#endif


