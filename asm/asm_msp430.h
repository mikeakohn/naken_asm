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

#ifndef _ASM_MSP430_H
#define _ASM_MSP430_H

#include "assembler.h"

int parse_instruction_msp430(struct _asm_context *asm_context, char *instr);

#endif

