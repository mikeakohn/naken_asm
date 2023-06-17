/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2023 by Michael Kohn, Lars Brinkhoff
 *
 * PDP-8 by Lars Brinkhoff
 *
 */

#ifndef NAKEN_ASM_DISASM_PDP8_H
#define NAKEN_ASM_DISASM_PDP8_H

#include "common/assembler.h"

int disasm_pdp8(Memory *memory, uint32_t address, char *instruction);

void list_output_pdp8(
  AsmContext *asm_context,
  uint32_t start,
  uint32_t end);

void disasm_range_pdp8(
  Memory *memory,
  uint32_t flags,
  uint32_t start,
  uint32_t end);

#endif

