/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2019 by Michael Kohn
 *
 */

#ifndef NAKEN_ASM_DISASM_DSPIC_H
#define NAKEN_ASM_DISASM_DSPIC_H

#include "common/assembler.h"

int get_cycle_count_dspic(unsigned short int opcode);
int disasm_dspic(struct _memory *memory, uint32_t address, char *instruction, int *cycles_min, int *cycles_max);
void list_output_dspic(struct _asm_context *asm_context, uint32_t start, uint32_t end);
void disasm_range_dspic(struct _memory *memory, uint32_t flags, uint32_t start, uint32_t end);

int get_dspic_flag_value(int flag);
char *get_dspic_flag_str(int flag);
int convert_dspic_flag_combo(int value, int flags);

#endif

