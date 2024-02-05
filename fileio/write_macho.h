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

#ifndef NAKEN_ASM_WRITE_MACHO_H
#define NAKEN_ASM_WRITE_MACHO_H

#include <stdio.h>
#include <stdlib.h>

#include "common/Memory.h"
#include "common/Symbols.h"

int write_macho(
  Memory *memory,
  FILE *out,
  Symbols *symbols,
  const char *filename,
  int cpu_type,
  int alignment);

#endif

