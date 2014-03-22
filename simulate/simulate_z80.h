/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2014 by Michael Kohn
 *
 */

#ifndef _SIMULATE_Z80_H
#define _SIMULATE_Z80_H

#include <unistd.h>

#include "memory.h"
#include "simulate_common.h"

struct _simulate_z80
{
  uint16_t reg[16];
};

struct _simulate *simulate_init_z80();
void simulate_free_z80(struct _simulate *simulate);
void simulate_push_z80(struct _simulate *simulate, uint32_t value);
int simulate_set_reg_z80(struct _simulate *simulate, char *reg_string, uint32_t value);
uint32_t simulate_get_reg_z80(struct _simulate *simulate, char *reg_string);
void simulate_reset_z80(struct _simulate *simulate);
void simulate_dump_registers_z80(struct _simulate *simulate);
int simulate_run_z80(struct _simulate *simulate, int max_cycles, int step);

#endif

