/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2012 by Michael Kohn
 *
 * 65xx file by Joe Davisson
 *
 */

#ifndef _DISASM_65XX_H
#define _DISASM_65XX_H

#include "assembler.h"

int get_cycle_count_65xx(unsigned short int opcode);
int disasm_65xx(struct _memory *memory, int address, char *instruction, int *cycles_min, int *cycles_max);
void list_output_65xx(struct _asm_context *asm_context, int address);

#endif

