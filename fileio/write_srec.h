/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2017 by Michael Kohn
 *
 */

#ifndef _WRITE_SREC_H
#define _WRITE_SREC_H

int write_srec(struct _memory *memory, FILE *out, int srec_size);

#endif

