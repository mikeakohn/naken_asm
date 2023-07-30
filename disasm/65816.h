/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2023 by Michael Kohn, Joe Davisson
 *
 * 65816 by Joe Davisson
 *
 */

#ifndef NAKEN_ASM_DISASM_65816_H
#define NAKEN_ASM_DISASM_65816_H

#include <stdint.h>

#include "common/assembler.h"

int disasm_65816(
  Memory *memory,
  uint32_t address,
  char *instruction,
  int length,
  int *cycles_min,
  int *cycles_max,
  int bytes);

void list_output_65816(
  AsmContext *asm_context,
  uint32_t start,
  uint32_t end);

void disasm_range_65816(
  Memory *memory,
  uint32_t flags,
  uint32_t start,
  uint32_t end);

#endif

