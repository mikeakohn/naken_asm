/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2019 by Michael Kohn, Joe Davisson
 *
 * 65816 by Joe Davisson
 *
 */

#ifndef NAKEN_ASM_DISASM_65816_H
#define NAKEN_ASM_DISASM_65816_H

#include <stdint.h>

#include "common/assembler.h"

int get_cycle_count_65816(uint16_t opcode);
int disasm_65816(struct _memory *memory, uint32_t address, char *instruction, int *cycles_min, int *cycles_max, int bytes);
void list_output_65816(struct _asm_context *asm_context, uint32_t start, uint32_t end);
void disasm_range_65816(struct _memory *memory, uint32_t flags, uint32_t start, uint32_t end);

#endif

