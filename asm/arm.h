/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2019 by Michael Kohn
 *
 */

#ifndef NAKEN_ASM_ASM_ARM_H
#define NAKEN_ASM_ASM_ARM_H

#include "common/assembler.h"

int parse_instruction_arm(struct _asm_context *asm_context, char *instr);

#endif

