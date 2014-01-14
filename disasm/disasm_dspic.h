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

#ifndef _DISASM_DSPIC_H
#define _DISASM_DSPIC_H

#include "assembler.h"

int get_cycle_count_dspic(unsigned short int opcode);
int disasm_dspic(struct _memory *memory, int address, char *instruction, int *cycles_min, int *cycles_max);
void list_output_dspic(struct _asm_context *asm_context, int address);

int get_dspic_flag_value(int flag);
char *get_dspic_flag_str(int flag);
int convert_dspic_flag_combo(int value, int flags);
void disasm_range_dspic(struct _memory *memory, int start, int end);

#endif

