/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2017 by Michael Kohn
 *
 */

#ifndef _DISASM_SUPER_FX_H
#define _DISASM_SUPER_FX_H

#include "common/assembler.h"

int disasm_super_fx(struct _memory *memory, uint32_t address, char *instruction, int *cycles_min, int *cycles_max);
void list_output_super_fx(struct _asm_context *asm_context, uint32_t start, uint32_t end);
void disasm_range_super_fx(struct _memory *memory, uint32_t flags, uint32_t start, uint32_t end);

#endif

