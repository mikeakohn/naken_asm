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

#ifndef NAKEN_ASM_DISASM_CELL_H
#define NAKEN_ASM_DISASM_CELL_H

#include "common/assembler.h"

int disasm_cell(
  Memory *memory,
  uint32_t address,
  char *instruction,
  int *cycles_min,
  int *cycles_max);

void list_output_cell(
  struct _asm_context *asm_context,
  uint32_t start,
  uint32_t end);

void disasm_range_cell(
  Memory *memory,
  uint32_t flags,
  uint32_t start,
  uint32_t end);

#endif

