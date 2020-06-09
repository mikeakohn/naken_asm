/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2020 by Michael Kohn
 *
 */

#ifndef NAKEN_ASM_DISASM_TMS1000_H
#define NAKEN_ASM_DISASM_TMS1000_H

#include "common/assembler.h"

int get_cycle_count_tms1000(unsigned short int opcode);
int disasm_tms1000(struct _memory *memory, uint32_t address, char *instruction, int *cycles_min, int *cycles_max);
void list_output_tms1000(struct _asm_context *asm_context, uint32_t start, uint32_t end);
void disasm_range_tms1000(struct _memory *memory, uint32_t flags, uint32_t start, uint32_t end);

int get_cycle_count_tms1100(unsigned short int opcode);
int disasm_tms1100(struct _memory *memory, uint32_t address, char *instruction, int *cycles_min, int *cycles_max);
void list_output_tms1100(struct _asm_context *asm_context, uint32_t start, uint32_t end);
void disasm_range_tms1100(struct _memory *memory, uint32_t flags, uint32_t start, uint32_t end);

#endif

