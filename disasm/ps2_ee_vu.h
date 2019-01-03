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

#ifndef NAKEN_ASM_DISASM_PS2_EE_VU_H
#define NAKEN_ASM_DISASM_PS2_EE_VU_H

#include "common/assembler.h"
#include "table/ps2_ee_vu.h"

#define PS2_EE_VU0 0
#define PS2_EE_VU1 1

int get_cycle_count_ps2_ee_vu(uint32_t opcode);
//int disasm_ps2_ee_vu(struct _memory *memory, uint32_t flags, uint32_t address, char *instruction, int *cycles_min, int *cycles_max, int is_lower);
void list_output_ps2_ee_vu(struct _asm_context *asm_context, uint32_t start, uint32_t end);
void disasm_range_ps2_ee_vu(struct _memory *memory, uint32_t flags, uint32_t start, uint32_t end);

#endif

