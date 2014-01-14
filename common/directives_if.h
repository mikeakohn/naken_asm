/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2013 by Michael Kohn
 *
 */

#ifndef _DIRECTIVES_IF_H
#define _DIRECTIVES_IF_H

int ifdef_ignore(struct _asm_context *asm_context);
int parse_ifdef_ignore(struct _asm_context *asm_context, int ignore_section);
int parse_ifdef(struct _asm_context *asm_context, int ifndef);
int parse_if(struct _asm_context *asm_context);

#endif

