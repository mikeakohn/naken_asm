/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2015 by Michael Kohn
 *
 */

#ifndef _DIRECTIVES_INCLUDE_H
#define _DIRECTIVES_INCLUDE_H

int binfile_parse(struct _asm_context *asm_context);
int include_parse(struct _asm_context *asm_context);
int include_add_path(struct _asm_context *asm_context, char *paths);

#endif

