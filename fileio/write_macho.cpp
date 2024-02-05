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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common/assembler.h"
#include "common/Symbols.h"
#include "fileio/write_elf.h"

int write_macho(
  Memory *memory,
  FILE *out,
  Symbols *symbols,
  const char *filename,
  int cpu_type,
  int alignment)
{

  return 0;
}

