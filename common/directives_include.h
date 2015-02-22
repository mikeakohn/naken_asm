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

int add_to_include_path(struct _asm_context *asm_context, char *paths);
int parse_binfile(struct _asm_context *asm_context);
int parse_include(struct _asm_context *asm_context);

#endif

