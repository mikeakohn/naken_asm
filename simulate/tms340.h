/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2019 by Michael Kohn
 *
 */

#ifndef NAKEN_ASM__SIMULATE_TMS9900_H
#define NAKEN_ASM__SIMULATE_TMS9900_H

#include <unistd.h>

#include "common/memory.h"
#include "simulate/common.h"

struct _simulate_tms340
{
  uint32_t pc;
  uint32_t a[15];
  uint32_t b[15];
  uint32_t sp;
  uint32_t st;
};

struct _simulate *simulate_init_tms340();
void simulate_free_tms340(struct _simulate *simulate);
int simulate_dumpram_tms340(struct _simulate *simulate, int start, int end);
void simulate_push_tms340(struct _simulate *simulate, uint32_t value);
int simulate_set_reg_tms340(struct _simulate *simulate, char *reg_string, uint32_t value);
uint32_t simulate_get_reg_tms340(struct _simulate *simulate, char *reg_string);
void simulate_set_pc_tms340(struct _simulate *simulate, uint32_t value);
void simulate_reset_tms340(struct _simulate *simulate);
void simulate_dump_registers_tms340(struct _simulate *simulate);
int simulate_run_tms340(struct _simulate *simulate, int max_cycles, int step);

#endif

