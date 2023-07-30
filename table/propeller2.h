/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2023 by Michael Kohn
 *
 */

#ifndef NAKEN_ASM_TABLE_PROPELLER2_H
#define NAKEN_ASM_TABLE_PROPELLER2_H

#include <stdint.h>

enum
{
  OP_NONE,
  OP_D,
  OP_NUM_D,
  OP_NUM_S,
  OP_NUM_SP,
  OP_N_1,
  OP_N_2,
  OP_N_3,
  OP_N_23,
  OP_BRANCH,
  OP_A,
  OP_P,
  OP_C,
  OP_Z,
};

enum
{
  LOGIC_NONE,
  LOGIC_AND,
  LOGIC_OR,
  LOGIC_XOR,
};

struct _table_propeller2
{
  const char *instr;
  uint32_t opcode;
  uint32_t mask;
  uint8_t operand_count;
  uint8_t operands[3];
  uint8_t wz    : 1;
  uint8_t wc    : 1;
  uint8_t wcz   : 1;
  uint8_t logic : 2;
  int8_t cycles8_min;
  int8_t cycles8_max;
  int8_t cycles8hub_min;
  int8_t cycles8hub_max;
  int8_t cycles16_min;
  int8_t cycles16_max;
};

struct _registers_propeller2
{
  const char *name;
  uint32_t value;
};

extern struct _table_propeller2 table_propeller2[];
extern struct _registers_propeller2 registers_propeller2[];
extern int registers_propeller2_len;

#endif

