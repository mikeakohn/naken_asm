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

#ifndef NAKEN_ASM_DIRECTIVES_INCLUDE_H
#define NAKEN_ASM_DIRECTIVES_INCLUDE_H

int binfile_parse(AsmContext *asm_context);
int include_parse(AsmContext *asm_context);
int include_add_path(AsmContext *asm_context, const char *paths);

#endif

