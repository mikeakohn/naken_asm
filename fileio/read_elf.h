/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2014 by Michael Kohn
 *
 */

#ifndef _READ_ELF_H
#define _READ_ELF_H

#include "memory.h"

int read_elf(char *filename, struct _memory *memory, uint8_t *cpu_type);

#endif

