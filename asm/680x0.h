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

#ifndef _ASM_680X0_H
#define _ASM_680X0_H

#include "common/assembler.h"

int parse_instruction_680x0(struct _asm_context *asm_context, char *instr);

#endif


