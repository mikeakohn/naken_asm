/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2015 by Michael Kohn
 *
 */

#ifndef _DISASM_680X0_H
#define _DISASM_680X0_H

#include "common/assembler.h"

int get_cycle_count_680x0(unsigned short int opcode);
int disasm_680x0(struct _memory *memory, uint32_t address, char *instruction, int *cycles_min, int *cycles_max);
void list_output_680x0(struct _asm_context *asm_context, uint32_t start, uint32_t end);
void disasm_range_680x0(struct _memory *memory, uint32_t start, uint32_t end);

#endif

