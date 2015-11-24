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

#ifndef _DISASM_MIPS_H
#define _DISASM_MIPS_H

#include "common/assembler.h"
#include "table/mips32.h"

extern struct _mips32_instr mips32_r_table[];
extern struct _mips32_instr mips32_i_table[];
extern struct _mips32_cop_instr mips32_cop_table[];

int get_cycle_count_mips32(unsigned short int opcode);
int disasm_mips32(struct _memory *memory, int address, char *instruction, int *cycles_min, int *cycles_max);
void list_output_mips32(struct _asm_context *asm_context, int address);
void disasm_range_mips32(struct _memory *memory, int start, int end);

#endif

