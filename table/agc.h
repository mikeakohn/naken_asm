/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2024 by Michael Kohn
 *
 */

// Apollo Guidance Computer.

#ifndef NAKEN_ASM_TABLE_AGC_H
#define NAKEN_ASM_TABLE_AGC_H

#include <stdint.h>

enum
{
  AGC_OP_NONE,
  AGC_OP_K10,
  AGC_OP_K12,
  AGC_OP_IO,
};

struct _table_agc
{
  const char *instr;
  uint16_t    opcode;
  uint16_t    mask;
  uint8_t     type;
  bool        is_extra_code;
  uint8_t     cycles;
};

extern struct _table_agc table_agc[];

#endif

