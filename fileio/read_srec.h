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

#ifndef _READ_SREC_H
#define _READ_SREC_H

#include "common/memory.h"

int read_srec(char *filename, struct _memory *memory);

#endif

