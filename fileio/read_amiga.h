/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2026 by Michael Kohn
 *
 */

#ifndef NAKEN_ASM_READ_AMIGA_H
#define NAKEN_ASM_READ_AMIGA_H

#include <stdint.h>

#include "core/Memory.h"

int read_amiga(const char *filename, Memory *memory);

#endif

