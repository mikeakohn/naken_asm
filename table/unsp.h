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

#ifndef NAKEN_ASM_TABLE_UNSP_H
#define NAKEN_ASM_TABLE_UNSP_H

#include <stdint.h>

enum
{
  UNSP_OP_NONE,
  UNSP_OP_GOTO,
  UNSP_OP_MUL,
  UNSP_OP_MAC,
  UNSP_OP_JMP,
  UNSP_OP_ALU,
  UNSP_OP_ALU_2,
  UNSP_OP_POP,
  UNSP_OP_PUSH,
};

struct _table_unsp
{
  const char *instr;
  uint16_t opcode;
  uint16_t mask;
  uint8_t type;
};

extern struct _table_unsp table_unsp[];

#endif

