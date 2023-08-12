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

#ifndef NAKEN_ASM_READ_HEX_H
#define NAKEN_ASM_READ_HEX_H

#include "common/Memory.h"

int read_hex(const char *filename, Memory *memory);

#endif

