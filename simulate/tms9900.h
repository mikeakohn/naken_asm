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

#ifndef NAKEN_ASM_SIMULATE_TMS9900_H
#define NAKEN_ASM_SIMULATE_TMS9900_H

#include <unistd.h>

#include "common/memory.h"
#include "simulate/common.h"

typedef struct _simulate_tms9900
{
  uint16_t pc;
  uint16_t wp;
  uint16_t st;
} SimulateTms9900;

Simulate *simulate_init_tms9900();
void simulate_free_tms9900(Simulate *simulate);
int simulate_dumpram_tms9900(Simulate *simulate, int start, int end);
void simulate_push_tms9900(Simulate *simulate, uint32_t value);
int simulate_set_reg_tms9900(Simulate *simulate, char *reg_string, uint32_t value);
uint32_t simulate_get_reg_tms9900(Simulate *simulate, char *reg_string);
void simulate_set_pc_tms9900(Simulate *simulate, uint32_t value);
void simulate_reset_tms9900(Simulate *simulate);
void simulate_dump_registers_tms9900(Simulate *simulate);
int simulate_run_tms9900(Simulate *simulate, int max_cycles, int step);

#endif

