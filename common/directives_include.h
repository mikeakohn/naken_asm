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

#ifndef NAKEN_ASM_DIRECTIVES_INCLUDE_H
#define NAKEN_ASM_DIRECTIVES_INCLUDE_H

int binfile_parse(struct _asm_context *asm_context);
int include_parse(struct _asm_context *asm_context);
int include_add_path(struct _asm_context *asm_context, char *paths);

#endif

