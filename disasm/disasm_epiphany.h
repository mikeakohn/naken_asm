/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2014 by Michael Kohn
 *
 */

#ifndef _DISASM_EPIPHANY_H
#define _DISASM_EPIPHANY_H

#include "assembler.h"

int get_cycle_count_epiphany(unsigned short int opcode);
int disasm_epiphany(struct _memory *memory, int address, char *instruction, int *cycles_min, int *cycles_max);
void list_output_epiphany(struct _asm_context *asm_context, int address);
void disasm_range_epiphany(struct _memory *memory, int start, int end);

#endif

