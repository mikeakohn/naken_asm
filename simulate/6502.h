/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2023 by Michael Kohn, Joe Davisson
 *
 * 6502 file by Joe Davisson
 *
 */

#ifndef NAKEN_ASM_SIMULATE_6502_H
#define NAKEN_ASM_SIMULATE_6502_H

#include <unistd.h>

#include "common/memory.h"
#include "simulate/common.h"

struct _simulate_6502
{
  // Define registers and anything 6502 specific here
  int reg_a, reg_x, reg_y, reg_sr, reg_pc, reg_sp;
};

struct _simulate *simulate_init_6502(struct _memory *memory);
void simulate_free_6502(struct _simulate *simulate);
int simulate_dumpram_6502(struct _simulate *simulate, int start, int end);
void simulate_push_6502(struct _simulate *simulate, uint32_t value);
int simulate_set_reg_6502(struct _simulate *simulate, char *reg_string, uint32_t value);
uint32_t simulate_get_reg_6502(struct _simulate *simulate, char *reg_string);
void simulate_set_pc_6502(struct _simulate *simulate, uint32_t value);
void simulate_reset_6502(struct _simulate *simulate);
void simulate_dump_registers_6502(struct _simulate *simulate);
int simulate_run_6502(struct _simulate *simulate, int max_cycles, int step);

#endif

