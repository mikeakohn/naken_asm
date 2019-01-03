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

#ifndef NAKEN_ASM_READ_BIN_H
#define NAKEN_ASM_READ_BIN_H

#include <stdint.h>

#include "common/memory.h"

int read_bin(char *filename, struct _memory *memory, uint32_t start_address);

#endif

