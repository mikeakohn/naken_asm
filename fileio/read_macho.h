/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2024 by Michael Kohn
 *
 */

#ifndef NAKEN_ASM_READ_MACHO_H
#define NAKEN_ASM_READ_MACHO_H

#include "common/Memory.h"
#include "common/Symbols.h"

int read_macho(
  const char *filename,
  Memory *memory,
  uint8_t *cpu_type,
  Symbols *symbols);

#endif

