/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2015 by Michael Kohn
 *
 * 65xx file by Joe Davisson
 *
 */

#ifndef _ASM_65XX_H
#define _ASM_65XX_H

#include "common/assembler.h"

int parse_instruction_65xx(struct _asm_context *asm_context, char *instr);

#endif


