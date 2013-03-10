/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2013 by Michael Kohn
 *
 */

#ifndef _DISASM__H
#define _DISASM__H

#include "assembler.h"

Replace NAME with cpu arch

int get_cycle_count_NAME(unsigned short int opcode);
int disasm_NAME(struct _memory *memory, int address, char *instruction, int *cycles_min, int *cycles_max);
void list_output_NAME(struct _asm_context *asm_context, int address);
void disasm_range_NAME(struct _memory *memory, int start, int end);

#endif

