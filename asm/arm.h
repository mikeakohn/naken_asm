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

#ifndef NAKEN_ASM_ASM_ARM_H
#define NAKEN_ASM_ASM_ARM_H

#include "core/assembler.h"

int parse_instruction_arm(AsmContext *asm_context, char *instr);

#endif

