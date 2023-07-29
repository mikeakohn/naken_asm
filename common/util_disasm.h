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

#ifndef UTIL_DISASM_H
#define UTIL_DISASM_H

#include "common/util_context.h"
#include "common/util_disasm.h"

void util_disasm(UtilContext *util_context, const char *token);
void util_disasm_range(UtilContext *util_context, int start, int end);

#endif

