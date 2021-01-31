/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2021 by Michael Kohn
 *
 */

#ifndef NAKEN_ASM_TABLE_68HC08
#define NAKEN_ASM_TABLE_68HC08

#include "stdint.h"

enum
{
  CPU08_OP_NONE,
  CPU08_OP_NUM16,
  CPU08_OP_NUM8,
  CPU08_OP_NUM8_OPR8,
  CPU08_OP_NUM8_REL,
  CPU08_OP_OPR16,
  CPU08_OP_OPR16_X,
  CPU08_OP_OPR8,
  CPU08_OP_OPR8_OPR8,
  CPU08_OP_OPR8_REL,
  CPU08_OP_OPR8_X,
  CPU08_OP_OPR8_X_PLUS,
  CPU08_OP_OPR8_X_PLUS_REL,
  CPU08_OP_OPR8_X_REL,
  CPU08_OP_REL,
  CPU08_OP_COMMA_X,
  CPU08_OP_X,
  CPU08_OP_X_PLUS_OPR8,
  CPU08_OP_X_PLUS_REL,
  CPU08_OP_X_REL,
  CPU08_OP_0_COMMA_OPR,
  CPU08_OP_1_COMMA_OPR,
  CPU08_OP_2_COMMA_OPR,
  CPU08_OP_3_COMMA_OPR,
  CPU08_OP_4_COMMA_OPR,
  CPU08_OP_5_COMMA_OPR,
  CPU08_OP_6_COMMA_OPR,
  CPU08_OP_7_COMMA_OPR,
  CPU08_OP_0_COMMA_OPR_REL,
  CPU08_OP_1_COMMA_OPR_REL,
  CPU08_OP_2_COMMA_OPR_REL,
  CPU08_OP_3_COMMA_OPR_REL,
  CPU08_OP_4_COMMA_OPR_REL,
  CPU08_OP_5_COMMA_OPR_REL,
  CPU08_OP_6_COMMA_OPR_REL,
  CPU08_OP_7_COMMA_OPR_REL,
  CPU08_OP_OPR8_SP,
  CPU08_OP_OPR8_SP_REL,
  CPU08_OP_OPR16_SP,
};

struct _m68hc08_table
{
  const char *instr;
  uint8_t type;
  uint8_t cycles;
};

struct _m68hc08_16_table
{
  const char *instr;
  uint16_t opcode;
  uint8_t type;
  uint8_t cycles;
  uint8_t has_16_bit_version;
};

extern struct _m68hc08_table m68hc08_table[];
extern struct _m68hc08_16_table m68hc08_16_table[];

#endif

