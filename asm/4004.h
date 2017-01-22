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

#ifndef _ASM_4004_H
#define _ASM_4004_H

#include "common/assembler.h"

int parse_instruction_4004(struct _asm_context *asm_context, char *instr);

#endif


