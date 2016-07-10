/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2016 by Michael Kohn
 *
 */

#ifndef _READ_ELF_H
#define _READ_ELF_H

#include "common/memory.h"
#include "common/symbols.h"

int read_elf(char *filename, struct _memory *memory, uint8_t *cpu_type, struct _symbols *symbols);

#endif

