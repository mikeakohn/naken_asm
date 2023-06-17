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

#ifndef NAKEN_ASM_DISASM_PS2_EE_VU_H
#define NAKEN_ASM_DISASM_PS2_EE_VU_H

#include "common/assembler.h"
#include "table/ps2_ee_vu.h"

#define PS2_EE_VU0 0
#define PS2_EE_VU1 1

#if 0
int disasm_ps2_ee_vu(
  Memory *memory,
  uint32_t flags,
  uint32_t address,
  char *instruction,
  int *cycles_min,
  int *cycles_max,
  int is_lower);
#endif

void list_output_ps2_ee_vu(
  AsmContext *asm_context,
  uint32_t start,
  uint32_t end);

void disasm_range_ps2_ee_vu(
  Memory *memory,
  uint32_t flags,
  uint32_t start,
  uint32_t end);

#endif

