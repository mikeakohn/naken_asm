/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2026 by Michael Kohn
 *
 */

#ifndef NAKEN_ASM_DISASM_8051_H
#define NAKEN_ASM_DISASM_8051_H

#include "common/assembler.h"

int disasm_8051(
  Memory *memory,
  uint32_t address,
  char *instruction,
  int length,
  int flags,
  int *cycles_min,
  int *cycles_max);

void list_output_8051(
  AsmContext *asm_context,
  uint32_t start,
  uint32_t end);

void disasm_range_8051(
  Memory *memory,
  uint32_t flags,
  uint32_t start,
  uint32_t end);

#endif

