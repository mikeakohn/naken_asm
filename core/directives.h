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

#ifndef NAKEN_ASM_DIRECTIVES_H
#define NAKEN_ASM_DIRECTIVES_H

#include "core/assembler.h"

int parse_org(AsmContext *asm_context);
int parse_directives(AsmContext *asm_context);

#endif

