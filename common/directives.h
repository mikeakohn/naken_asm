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

#ifndef NAKEN_ASM_DIRECTIVES_H
#define NAKEN_ASM_DIRECTIVES_H

#include "common/assembler.h"

int parse_org(AsmContext *asm_context);
int parse_directives(AsmContext *asm_context);

#endif

