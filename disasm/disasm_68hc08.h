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

#ifndef _DISASM_68HC08_H
#define _DISASM_68HC08_H

#include "assembler.h"


int get_cycle_count_68hc08(unsigned short int opcode);
int disasm_68hc08(struct _memory *memory, int address, char *instruction, int *cycles_min, int *cycles_max);
void list_output_68hc08(struct _asm_context *asm_context, int address);
void disasm_range_68hc08(struct _memory *memory, int start, int end);

#endif

