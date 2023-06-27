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

#ifndef NAKEN_ASM_TABLE_EBPF_H
#define NAKEN_ASM_TABLE_EBPF_H

#include "common/assembler.h"

enum
{
  OP_NONE,
  OP_IMM,
  OP_REG,
  OP_REG_REG,
  OP_REG_IMM,
};

enum
{
  SIZE_NONE = 0,
  SIZE_EITHER = 1,
  SIZE_32 = 32,
  SIZE_64 = 64
};

struct _table_ebpf
{
  char *instr;
  uint8_t opcode;
  uint8_t type;
  uint8_t size;
};

extern struct _table_ebpf table_ebpf[];

#endif

