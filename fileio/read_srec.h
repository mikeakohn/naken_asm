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

#ifndef NAKEN_ASM_READ_SREC_H
#define NAKEN_ASM_READ_SREC_H

#include "common/memory.h"

int read_srec(char *filename, struct _memory *memory);

#endif

