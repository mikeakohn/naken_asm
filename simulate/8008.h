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

#ifndef NAKEN_ASM_SIMULATE_8008_H
#define NAKEN_ASM_SIMULATE_8008_H

#include <unistd.h>

#include "common/memory.h"
#include "simulate/common.h"

typedef struct _simulate_8008
{
  uint16_t reg[8];
  uint16_t pc;
} Simulate8008;

Simulate *simulate_init_8008(Memory *memory);
void simulate_free_8008(Simulate *simulate);
int simulate_dumpram_8008(Simulate *simulate, int start, int end);
void simulate_push_8008(Simulate *simulate, uint32_t value);
int simulate_set_reg_8008(Simulate *simulate, const char *reg_string, uint32_t value);
uint32_t simulate_get_reg_8008(Simulate *simulate, const char *reg_string);
void simulate_set_pc_8008(Simulate *simulate, uint32_t value);
void simulate_reset_8008(Simulate *simulate);
void simulate_dump_registers_8008(Simulate *simulate);
int simulate_run_8008(Simulate *simulate, int max_cycles, int step);

#endif

