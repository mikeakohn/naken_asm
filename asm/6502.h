/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2017 by Michael Kohn, Joe Davisson
 *
 * 6502 file by Joe Davisson
 *
 */

#ifndef _ASM_6502_H
#define _ASM_6502_H

#include "common/assembler.h"

int parse_instruction_6502(struct _asm_context *asm_context, char *instr);

#endif


