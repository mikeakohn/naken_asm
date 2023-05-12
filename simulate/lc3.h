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

#ifndef NAKEN_ASM_SIMULATE_LC3_H
#define NAKEN_ASM_SIMULATE_LC3_H

#include <unistd.h>

#include "common/memory.h"
#include "simulate/common.h"

#define FLAG_PRIV 0x8000
#define FLAG_N 0x0004
#define FLAG_Z 0x0002
#define FLAG_P 0x0001

struct _simulate_lc3
{
  uint16_t reg[8];
  uint16_t pc;
  uint16_t psr;
};

struct _simulate *simulate_init_lc3();
void simulate_free_lc3(struct _simulate *simulate);
int simulate_dumpram_lc3(struct _simulate *simulate, int start, int end);
void simulate_push_lc3(struct _simulate *simulate, uint32_t value);
int simulate_set_reg_lc3(struct _simulate *simulate, char *reg_string, uint32_t value);
uint32_t simulate_get_reg_lc3(struct _simulate *simulate, char *reg_string);
void simulate_set_pc_lc3(struct _simulate *simulate, uint32_t value);
void simulate_reset_lc3(struct _simulate *simulate);
void simulate_dump_registers_lc3(struct _simulate *simulate);
int simulate_run_lc3(struct _simulate *simulate, int max_cycles, int step);

#endif

