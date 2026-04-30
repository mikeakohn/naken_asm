/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2026 by Michael Kohn, Lars Brinkhoff
 *
 * PDP-8 by Lars Brinkhoff
 *
 */

#ifndef NAKEN_ASM_ASM_PDP8_H
#define NAKEN_ASM_ASM_PDP8_H

#include "core/assembler.h"

int parse_instruction_pdp8(AsmContext *asm_context, char *instr);

#endif

