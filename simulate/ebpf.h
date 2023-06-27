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

#ifndef NAKEN_ASM_SIMULATE_EBPF_H
#define NAKEN_ASM_SIMULATE__H

#include <unistd.h>

#include "common/memory.h"
#include "simulate/common.h"

typedef struct _simulate_ebpf
{
  int64_t reg[16];
  uint32_t pc;

} SimulateEbpf;

Simulate *simulate_init_ebpf();
void simulate_free_ebpf(Simulate *simulate);
int simulate_dumpram_ebpf(Simulate *simulate, int start, int end);
void simulate_push_ebpf(Simulate *simulate, uint32_t value);
int simulate_set_reg_ebpf(Simulate *simulate, char *reg_string, uint32_t value);
uint32_t simulate_get_reg_ebpf(Simulate *simulate, char *reg_string);
void simulate_set_pc_ebpf(Simulate *simulate, uint32_t value);
void simulate_reset_ebpf(Simulate *simulate);
void simulate_dump_registers_ebpf(Simulate *simulate);
int simulate_run_ebpf(Simulate *simulate, int max_cycles, int step);

#endif

