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

#ifndef NAKEN_ASM_DISASM_MSP430_H
#define NAKEN_ASM_DISASM_MSP430_H

#include <stdint.h>

#include "common/assembler.h"

int get_register_msp430(char *token);
int get_cycle_count(uint16_t opcode);

int disasm_msp430(
  Memory *memory,
  uint32_t address,
  char *instruction,
  int *cycles_min,
  int *cycles_max);

void list_output_msp430(
  AsmContext *asm_context,
  uint32_t start,
  uint32_t end);

void list_output_msp430x(
  AsmContext *asm_context,
  uint32_t start,
  uint32_t end);

void disasm_range_msp430(
  Memory *memory,
  uint32_t flags,
  uint32_t start,
  uint32_t end);

void disasm_range_msp430x(
  Memory *memory,
  uint32_t flags,
  uint32_t start,
  uint32_t end);

#endif

