/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2023 by Michael Kohn
 *
 */

#ifndef NAKEN_ASM_ASM_68HC08_H
#define NAKEN_ASM_ASM_68HC08_H

#include "common/assembler.h"

int parse_instruction_68hc08(AsmContext *asm_context, char *instr);

#endif

