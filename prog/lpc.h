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

#ifndef NAKEN_ASM_LPC_H
#define NAKEN_ASM_LPC_H

#include "common/memory.h"

int lpc_info(char *device);
int lpc_memory_read(char *device, struct _memory *memory, uint32_t address, uint32_t count);
int lpc_memory_write(char *device, struct _memory *memory);
int lpc_run(char *device, uint32_t address);

#endif

