/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2017 by Michael Kohn
 *
 * 65816 file by Joe Davisson
 *
 */

#ifndef _SIMULATE_65816_H
#define _SIMULATE_65816_H

#include <unistd.h>

#include "common/memory.h"
#include "simulate/common.h"

struct _simulate_65816
{
  // Define registers and anything 65816 specific here
  int reg_a, reg_x, reg_y, reg_sr, reg_pc, reg_sp, reg_db, reg_pb;
};

struct _simulate *simulate_init_65816(struct _memory *memory);
void simulate_free_65816(struct _simulate *simulate);
int simulate_dumpram_65816(struct _simulate *simulate, int start, int end);
void simulate_push_65816(struct _simulate *simulate, uint32_t value);
int simulate_set_reg_65816(struct _simulate *simulate, char *reg_string, unsigned int value);
uint32_t simulate_get_reg_65816(struct _simulate *simulate, char *reg_string);
void simulate_set_pc_65816(struct _simulate *simulate, uint32_t value);
void simulate_reset_65816(struct _simulate *simulate);
void simulate_dump_registers_65816(struct _simulate *simulate);
int simulate_run_65816(struct _simulate *simulate, int max_cycles, int step);

#endif

