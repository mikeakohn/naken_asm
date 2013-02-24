/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2012 by Michael Kohn
 *
 */

#ifndef _DISASM_680X_H
#define _DISASM_680X_H

#include "assembler.h"


int get_cycle_count_680x(unsigned short int opcode);
int disasm_680x(struct _memory *memory, int address, char *instruction, int *cycles_min, int *cycles_max);
void list_output_680x(struct _asm_context *asm_context, int address);
void disasm_range_680x(struct _memory *memory, int start, int end);

#endif

