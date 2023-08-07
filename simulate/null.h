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

#ifndef NAKEN_ASM_SIMULATE_NULL_H
#define NAKEN_ASM_SIMULATE_NULL_H

#include <unistd.h>

#include "common/memory.h"
#include "simulate/Simulate.h"

typedef struct _simulate_null
{
  uint16_t reg[16];
} SimulateNull;

Simulate *simulate_init_null(Memory *memory);
void simulate_free_null(Simulate *simulate);
int simulate_dumpram_null(Simulate *simulate, int start, int end);
void simulate_push_null(Simulate *simulate, uint32_t value);
int simulate_set_reg_null(Simulate *simulate, const char *reg_string, uint32_t value);
uint32_t simulate_get_reg_null(Simulate *simulate, const char *reg_string);
void simulate_set_pc_null(Simulate *simulate, uint32_t value);
void simulate_reset_null(Simulate *simulate);
void simulate_dump_registers_null(Simulate *simulate);
int simulate_run_null(Simulate *simulate, int max_cycles, int step);

#endif

