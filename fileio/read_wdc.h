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

#ifndef NAKEN_ASM_READ_WDC_H
#define NAKEN_ASM_READ_WDC_H

#include <stdint.h>

#include "common/memory.h"

int read_wdc(const char *filename, Memory *memory);

#endif

