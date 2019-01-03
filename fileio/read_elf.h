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

#ifndef NAKEN_ASM_READ_ELF_H
#define NAKEN_ASM_READ_ELF_H

#include "common/memory.h"
#include "common/symbols.h"

int read_elf(char *filename, struct _memory *memory, uint8_t *cpu_type, struct _symbols *symbols);

#endif

