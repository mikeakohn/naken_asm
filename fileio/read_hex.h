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

#ifndef _READ_HEX_H
#define _READ_HEX_H

#include "common/memory.h"

int read_hex(char *filename, struct _memory *memory);

#endif

