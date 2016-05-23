/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2016 by Michael Kohn
 *
 */

#ifndef _DISASM_PS2_EE_VU_H
#define _DISASM_PS2_EE_VU_H

#include "common/assembler.h"
#include "table/ps2_ee_vu.h"

int get_cycle_count_ps2_ee_vu(uint32_t opcode);
int disasm_ps2_ee_vu(struct _memory *memory, uint32_t address, char *instruction, int *cycles_min, int *cycles_max);
void list_output_ps2_ee_vu(struct _asm_context *asm_context, uint32_t start, uint32_t end);
void disasm_range_ps2_ee_vu(struct _memory *memory, uint32_t start, uint32_t end);

#endif

