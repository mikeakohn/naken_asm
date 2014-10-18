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

#ifndef _SIMULATE_NULL_H
#define _SIMULATE_NULL_H

#include <unistd.h>

#include "memory.h"
#include "simulate_common.h"

struct _simulate_null
{
  uint16_t reg[16];
};

struct _simulate *simulate_init_null();
void simulate_free_null(struct _simulate *simulate);
int simulate_dumpram_null(struct _simulate *simulate, int start, int end);
void simulate_push_null(struct _simulate *simulate, uint32_t value);
int simulate_set_reg_null(struct _simulate *simulate, char *reg_string, uint32_t value);
uint32_t simulate_get_reg_null(struct _simulate *simulate, char *reg_string);
void simulate_reset_null(struct _simulate *simulate);
void simulate_dump_registers_null(struct _simulate *simulate);
int simulate_run_null(struct _simulate *simulate, int max_cycles, int step);

#endif

