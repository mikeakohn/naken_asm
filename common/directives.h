/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2021 by Michael Kohn
 *
 */

#ifndef NAKEN_ASM_DIRECTIVES_H
#define NAKEN_ASM_DIRECTIVES_H

#include "common/assembler.h"

int parse_org(struct _asm_context *asm_context);
int parse_directives(struct _asm_context *asm_context);

#endif

