/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2023 by Michael Kohn, Joe Davisson
 *
 * 6502 file by Joe Davisson
 *
 */

#ifndef NAKEN_ASM_ASM_6502_H
#define NAKEN_ASM_ASM_6502_H

#include "common/assembler.h"

int parse_instruction_6502(AsmContext *asm_context, char *instr);

#endif

