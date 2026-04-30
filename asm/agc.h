/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2026 by Michael Kohn
 *
 */

// Apollo Guidance Computer.

#ifndef NAKEN_ASM_ASM_AGC_H
#define NAKEN_ASM_ASM_AGC_H

#include "core/assembler.h"

int parse_instruction_agc(AsmContext *asm_context, char *instr);

#endif

