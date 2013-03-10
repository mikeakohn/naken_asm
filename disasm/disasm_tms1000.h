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

#ifndef _DISASM_TMS1000_H
#define _DISASM_TMS1000_H

#include "assembler.h"

struct _tms_instr
{
  const char *instr;
  unsigned short int op1000;
  unsigned short int op1100;
};


extern struct _tms_instr tms_instr_table[];

int get_cycle_count_tms1000(unsigned short int opcode);
int disasm_tms1000(struct _memory *memory, int address, char *instruction, int *cycles_min, int *cycles_max);
void list_output_tms1000(struct _asm_context *asm_context, int address);
void disasm_range_tms1000(struct _memory *memory, int start, int end);

int get_cycle_count_tms1100(unsigned short int opcode);
int disasm_tms1100(struct _memory *memory, int address, char *instruction, int *cycles_min, int *cycles_max);
void list_output_tms1100(struct _asm_context *asm_context, int address);
void disasm_range_tms1100(struct _memory *memory, int start, int end);

#endif

