/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2017 by Michael Kohn
 *
 */

#ifndef _SIMULATE_NULL_H
#define _SIMULATE_NULL_H

#include <unistd.h>

#include "common/memory.h"
#include "simulate/common.h"

struct _simulate_mips
{
  int32_t reg[32];
  uint32_t pc;
  uint32_t hi;
  uint32_t lo;
};

struct _simulate *simulate_init_mips();
void simulate_free_mips(struct _simulate *simulate);
int simulate_dumpram_mips(struct _simulate *simulate, int start, int end);
void simulate_push_mips(struct _simulate *simulate, uint32_t value);
int simulate_set_reg_mips(struct _simulate *simulate, char *reg_string, uint32_t value);
uint32_t simulate_get_reg_mips(struct _simulate *simulate, char *reg_string);
void simulate_set_pc_mips(struct _simulate *simulate, uint32_t value);
void simulate_reset_mips(struct _simulate *simulate);
void simulate_dump_registers_mips(struct _simulate *simulate);
int simulate_run_mips(struct _simulate *simulate, int max_cycles, int step);

#endif

