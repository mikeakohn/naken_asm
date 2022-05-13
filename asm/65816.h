/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2022 by Michael Kohn, Joe Davisson
 *
 * 65816 by Joe Davisson
 *
 */

#ifndef NAKEN_ASM_ASM_65816_H
#define NAKEN_ASM_ASM_65816_H

#include "common/assembler.h"

int parse_directive_65816(
  struct _asm_context *asm_context,
  const char *directive);

int parse_instruction_65816(struct _asm_context *asm_context, char *instr);

#endif

