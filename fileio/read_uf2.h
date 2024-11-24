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

#ifndef NAKEN_ASM_READ_UF2_H
#define NAKEN_ASM_READ_UF2_H

#include <stdint.h>

#include "common/Memory.h"

int read_uf2(const char *filename, Memory *memory);

#endif

