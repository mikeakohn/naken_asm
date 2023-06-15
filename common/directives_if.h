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

#ifndef NAKEN_ASM_DIRECTIVES_IF_H
#define NAKEN_ASM_DIRECTIVES_IF_H

int ifdef_ignore(AsmContext *asm_context);
int parse_ifdef_ignore(AsmContext *asm_context, int ignore_section);
int parse_ifdef(AsmContext *asm_context, int ifndef);
int parse_if(AsmContext *asm_context);

#endif

