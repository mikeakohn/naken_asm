/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2014 by Michael Kohn, Joe Davisson
 *
 * 65816 by Joe Davisson
 *
 */

#ifndef _DISASM_65816_H
#define _DISASM_65816_H

#include <stdint.h>

#include "assembler.h"

int get_cycle_count_65816(uint16_t opcode);
int disasm_65816(struct _memory *memory, int address, char *instruction, int *cycles_min, int *cycles_max);
void list_output_65816(struct _asm_context *asm_context, int address);
void disasm_range_65816(struct _memory *memory, int start, int end);

#endif

