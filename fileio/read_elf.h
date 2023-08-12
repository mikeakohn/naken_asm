/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2023 by Michael Kohn
 *
 */

#ifndef NAKEN_ASM_READ_ELF_H
#define NAKEN_ASM_READ_ELF_H

#include "common/memory.h"
#include "common/Symbols.h"

int read_elf(
  const char *filename,
  Memory *memory,
  uint8_t *cpu_type,
  Symbols *symbols);

#endif

