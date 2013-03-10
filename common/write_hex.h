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

#ifndef _WRITE_HEX_H
#define _WRITE_HEX_H

int write_hex(struct _asm_context *asm_context, FILE *out);
int write_bin(struct _asm_context *asm_context, FILE *out);

#endif

