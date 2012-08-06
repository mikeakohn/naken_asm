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

#ifndef _DISASM_MSP430_H
#define _DISASM_MSP430_H

#include "assembler.h"

int get_register_msp430(char *token);
int get_cycle_count(unsigned short int opcode);
int disasm_msp430(struct _memory *memory, int address, char *instruction, int *cycles_min, int *cycles_max);
void list_output_msp430(struct _asm_context *asm_context, int address);
void disasm_range_msp430(struct _memory *memory, int start, int end);

#endif

