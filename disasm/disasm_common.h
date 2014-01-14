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

#ifndef _DISASM_COMMON_H
#define _DISASM_COMMON_H

#include "assembler.h"

unsigned int get_opcode16(struct _memory *memory, int address);
//unsigned int get_opcode24(struct _memory *memory, int address);
unsigned int get_opcode32(struct _memory *memory, int address);

#endif

