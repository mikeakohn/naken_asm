/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2013 by Michael Kohn
 *
 */

#ifndef _LPC_H
#define _LPC_H

#include "memory.h"

int lpc_info(char *device);
int lpc_memory_read(char *device, struct _memory *memory, uint32_t address, uint32_t count);
int lpc_memory_write(char *device, struct _memory *memory);
int lpc_run(char *device, uint32_t address);

#endif

