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

#ifndef NAKEN_ASM_DIRECTIVES_IF_H
#define NAKEN_ASM_DIRECTIVES_IF_H

int ifdef_ignore(struct _asm_context *asm_context);
int parse_ifdef_ignore(struct _asm_context *asm_context, int ignore_section);
int parse_ifdef(struct _asm_context *asm_context, int ifndef);
int parse_if(struct _asm_context *asm_context);

#endif

