/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2019 by Michael Kohn
 *
 */

#ifndef NAKEN_ASM_DISASM__H
#define NAKEN_ASM_DISASM__H

#include "common/assembler.h"

Replace NAME with cpu arch

int get_cycle_count_NAME(unsigned short int opcode);
int disasm_NAME(struct _memory *memory, uint32_t address, char *instruction, int *cycles_min, int *cycles_max);
void list_output_NAME(struct _asm_context *asm_context, uint32_t start, uint32_t end);
void disasm_range_NAME(struct _memory *memory, uint32_t flags, uint32_t start, uint32_t end);

#endif

