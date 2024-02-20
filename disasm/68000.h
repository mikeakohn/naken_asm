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

#ifndef NAKEN_ASM_DISASM_68000_H
#define NAKEN_ASM_DISASM_68000_H

#include "common/assembler.h"

int disasm_68000(
  Memory *memory,
  uint32_t address,
  char *instruction,
  int length,
  int flags,
  int *cycles_min,
  int *cycles_max);

void list_output_68000(
  AsmContext *asm_context,
  uint32_t start,
  uint32_t end);

void disasm_range_68000(
  Memory *memory,
  uint32_t flags,
  uint32_t start,
  uint32_t end);

#endif

