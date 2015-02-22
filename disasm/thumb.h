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

#ifndef _DISASM_THUMB_H
#define _DISASM_THUMB_H

#include "common/assembler.h"

int get_cycle_count_thumb(unsigned short int opcode);
int disasm_thumb(struct _memory *memory, int address, char *instruction, int *cycles_min, int *cycles_max);
void list_output_thumb(struct _asm_context *asm_context, int address);
void disasm_range_thumb(struct _memory *memory, int start, int end);

#endif

