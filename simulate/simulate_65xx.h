/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2014 by Michael Kohn
 *
 * 65xx file by Joe Davisson
 *
 */

#ifndef _SIMULATE_65XX_H
#define _SIMULATE_65XX_H

#include <unistd.h>

#include "memory.h"
#include "simulate_common.h"

struct _simulate_65xx
{
  // Define registers and anything 65xx specific here
  int reg_a, reg_x, reg_y, reg_sr, reg_pc, reg_sp;
};

struct _simulate *simulate_init_65xx(struct _memory *memory);
void simulate_free_65xx(struct _simulate *simulate);
void simulate_push_65xx(struct _simulate *simulate, unsigned int value);
int simulate_set_reg_65xx(struct _simulate *simulate, char *reg_string, unsigned int value);
unsigned int simulate_get_reg_65xx(struct _simulate *simulate, char *reg_string);
void simulate_reset_65xx(struct _simulate *simulate);
void simulate_dump_registers_65xx(struct _simulate *simulate);
int simulate_run_65xx(struct _simulate *simulate, int max_cycles, int step);

#endif

